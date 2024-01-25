#include <iostream>
#include <sqlite3.h>
#include <format>

#include <cstdio>
#ifdef __MACH__
#include <mach/clock.h>
#include <mach/mach.h>
#endif
#include <ctime>
#include <chrono>
#include <algorithm>

#include "SHARED/loguru-master/loguru.hpp"
#include "SHARED/cpp-httplib-master/httplib.h"
#include "SHARED/json11-master/json11.hpp"
#include "SHARED/magic_enum-master/include/magic_enum.hpp"

#include <fmt/core.h>

#include "helper.h"
#include "SQLITE3_QUERY.hpp"
#include "SQLITE3.hpp"
#include "ConfigParser.h"

#define LOG_FILE_PATH_ERROR "./atcserver_error_log.log"
#define LOG_FILE_PATH "./atcserver_log.log"
#define DEFAULT_CONFIG_FILE_PATH "./atcserverconfig.ini"
#define GAMEDATA_DATABASE_NAME "gamedata"
#define SYSTEM_TICK_DELAY 100

#ifndef VERSION
    #define VERSION "1.0.0"
#endif


volatile bool main_thread_running = false;

void signal_callback_handler(int signum) {
    printf("Caught signal %d\n", signum);

    LOG_F(ERROR, "Caught signal %d\n", signum);
    loguru::flush();
    main_thread_running = false;
    //exit(signum);
}


typedef std::chrono::high_resolution_clock Clock;
typedef std::chrono::system_clock::time_point TimePoint;
#define CHRONO_DURATION_MS(x, y) std::chrono::duration_cast<std::chrono::milliseconds>(x - y).count()


#ifdef __MACH__
struct timespec t1;
struct timespec t2;
#else
TimePoint t1 = Clock::now();
    TimePoint t2 = Clock::now();
#endif

void sys_tick_delay(){
#ifdef __MACH__
    clock_serv_t cclock;
    mach_timespec_t mts;
    host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
    clock_get_time(cclock, &mts);
    mach_port_deallocate(mach_task_self(), cclock);
    t2.tv_sec = mts.tv_sec;
    t2.tv_nsec = mts.tv_nsec;
    // IF TIME DIFFERENCE IS BIG ENOUGHT => CALL THE TIMER EVENT
    if ((t2.tv_sec - t1.tv_sec) > SYSTEM_TICK_DELAY)
    {
        host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
        clock_get_time(cclock, &mts);
        mach_port_deallocate(mach_task_self(), cclock);
        t1.tv_sec = mts.tv_sec;
        t1.tv_nsec = mts.tv_nsec;
#else
        t2 = Clock::now();
        // IF TIME DIFFERENCE IS BIG ENOUGHT => CALL THE TIMER EVENT
        if (CHRONO_DURATION_MS(t2, t1) > SYSTEM_TICK_DELAY) {
            t1 = Clock::now();
#endif
    }
}


enum class GAME_STATE {
    PS_INVALID=0,
    PS_IDLE=1,
    PS_SEARCHING=4,
    PS_SEARCHING_MANUAL =5,
    PS_PREPARING_INGAME = 6,
    PS_INGAME = 7
};




int main(int argc, char *argv[]) {
    // REGISTER SIGNAL HANDLER
    signal(SIGINT, signal_callback_handler);

    //---- PRINT HELP MESSAGE ------------------------------------------------------ //
    if (cmdOptionExists(argv, argv + argc, "--help")) {
        std::cout << "---- ATC_SERVER HELP ----" << std::endl;
        std::cout << "--help                   | prints this message" << std::endl;
        std::cout << "--version                | prints a version message" << std::endl;
        std::cout << "--cfgfolderpath          | set the ABS folder path for the config FORMAT: /xyz/folder/ which contains atcserverconfig.ini, if not specified ./atcserverconfig.ini is used" << std::endl;
        std::cout << "--reset                  | resets game database" << std::endl;
        std::cout << "---- END ATC_SERVER HELP ----" << std::endl;
        return 0;
    }
    if (cmdOptionExists(argv, argv + argc, "--version")) {
        std::cout << "---- ATC_SERVER VERSION ----" << std::endl;
        std::cout << "atc_server version:" << VERSION << std::endl;
        std::cout << "atc_server build date:" << __DATE__ << std::endl;
        std::cout << "atc_server build date:" << __TIME__ << std::endl;
        return 0;
    }

    // SETUP LOGGER
    loguru::init(argc, argv);
    loguru::add_file(LOG_FILE_PATH, loguru::Truncate, loguru::Verbosity_MAX);
    loguru::add_file(LOG_FILE_PATH_ERROR, loguru::Truncate, loguru::Verbosity_WARNING);
    loguru::g_stderr_verbosity = 1;
    LOG_SCOPE_F(INFO, "ATC SERVER STARTED");

    // CREATE CONFIG FILE PATH
    std::string CONFIG_FILE_PATH = DEFAULT_CONFIG_FILE_PATH;
    //CHECKK FOLDER EXISTS
    // atccontrollerconfig
    if (cmdOptionExists(argv, argv + argc, "--cfgfolderpath")) {
        std::string fp = std::string(getCmdOption(argv, argv + argc, "--cfgfolderpath"));
        LOG_SCOPE_F(INFO, "LOADING CONFIG FILE: --cfgfolderpath IS SET TO %s", fp.c_str());
        if (fp.size() > 0) {
            if (fp.at(fp.size() - 1) != '/') {
                fp.append("/");
            }
        } else {
            fp = "./";
        }
        CONFIG_FILE_PATH = fp + "atcserverconfig.ini";
    }
    LOG_SCOPE_F(INFO, "LOADING CONFIG FILE %s", CONFIG_FILE_PATH.c_str());

    // LOAD CONFIG FILE
    if (!ConfigParser::getInstance()->loadConfigFile(CONFIG_FILE_PATH)){
        ConfigParser::getInstance()->loadDefaults("");
        ConfigParser::getInstance()->createConfigFile(CONFIG_FILE_PATH, true);
        LOG_F(INFO, "WRITE NEW CONFIGFILE DUE MISSING ONE: atcserverconfig.ini");
    }
    LOG_F(INFO, "CONFIG FILE LOADED");


    //LOAD DATABASE
    SQLITE3 db(ConfigParser::getInstance()->get(ConfigParser::CFG_ENTRY::DB_FILEPATH)); // init database


    //INIT DATABASE
    if (cmdOptionExists(argv, argv + argc, "--reset")) {
        db.execute(fmt::format("DROP TABLE IF EXISTS {};", GAMEDATA_DATABASE_NAME).c_str());
        db.commit();
        LOG_F(INFO, "GAME DATA TABLE DROPPED DUE SET --reset FLAG");
    }

    // CREATE DATA TABLE IF NOT EXISTS
   if (db.execute(fmt::format("CREATE TABLE IF NOT EXISTS {} (HWID TEXT NOT NULL UNIQUE,SID TEXT, game_state INTEGER,remote_player_is_white INTEGER, is_syncing_phase INTEGER,current_board_fen TEXT, PRIMARY KEY(HWID));", GAMEDATA_DATABASE_NAME).c_str())) {
        db.perror(); // in case of error, perror will print detail to stderr
   }
    db.commit();


   // INIT WEBSERVER
    httplib::Server svr;

    if (!svr.is_valid()) {
        LOG_F(INFO, "CANT START WEBSERVER");
        return -1;
    }
    //SET WEBSERVER LOGGING
    svr.set_logger([](const httplib::Request &req, const httplib::Response &res) {
        LOG_F(INFO, "%s", fmt::format("req: {} res {}", req.body, res.body).c_str());
    });
    //SET ERROR HANDLER
    svr.set_error_handler([](const httplib::Request & /*req*/, httplib::Response &res) {
        const char *fmt = "<p>Error Status: <span style='color:red;'>%d</span></p>";
        char buf[BUFSIZ];
        snprintf(buf, sizeof(buf), fmt, res.status);
        res.set_content(buf, "text/html");
    });

    // SERVER STATIC FILES
    auto ret = svr.set_mount_point("/public", ConfigParser::getInstance()->get(ConfigParser::CFG_ENTRY::STATIC_HTML_PATH).c_str());
    if (!ret) {
        LOG_F(ERROR, "set STATIC_HTML_PATH path cant be accessed");
    }

    // REGISTER ROUTES
    svr.Get("/", [](const httplib::Request& req, httplib::Response &res) {
        res.set_redirect("/public/index.html");
    });

    svr.Get("/rest/logout", [&db](const httplib::Request& req, httplib::Response &res) {

        bool err = false;
        std::string reason = "ok";
        // RESET PLAYER STATE
        const std::string hwid = sanitize_r(req.get_param_value("hwid"));
        const int initial_gamestate = magic_enum::enum_integer(GAME_STATE::PS_INVALID);
        SQLITE3_QUERY update_query = SQLITE3_QUERY(fmt::format("UPDATE {} SET SID='{}', game_state='{}', remote_player_is_white='{}', is_syncing_phase='{}', current_board_fen='{}' WHERE HWID='{}';", GAMEDATA_DATABASE_NAME, "", initial_gamestate, 0, 0, "", hwid)); // ? will be replaced after bind
        if (db.execute(update_query) && db.commit()) { // execute query
            db.perror();
            err = true;
            reason = "db update error";
        }

        json11::Json response_json = json11::Json::object {
                { "err", err },
                { "status", reason },
        };
        res.set_content(response_json.dump(), "application/json");
    });

    svr.Get("/rest/login", [&db](const httplib::Request& req, httplib::Response &res) {

        bool err = false;
        std::string reason = "ok";

        const std::string hwid = sanitize_r(req.get_param_value("hwid"));
        const std::string playertype = sanitize_r(req.get_param_value("playertype"));

        //GENERATE SID :)
        std::string sid = hwid;
        std::reverse(sid.begin(), sid.end());

        // CHECK INPUT
        if(hwid.empty()){
            err = true;
            reason = "hwid.size() <= 0";
        }else{


            const int initial_gamestate = magic_enum::enum_integer(GAME_STATE::PS_IDLE);
            const int remote_player_is_white = randomBoolean() * 1;
            const std::string initial_fen = ConfigParser::getInstance()->get(ConfigParser::CFG_ENTRY::INITIAL_BOARD_FEN);

            //CREATE NEW USER IF NOT EXISTS
            SQLITE3_QUERY query = SQLITE3_QUERY(fmt::format("SELECT * FROM {} WHERE HWID='{}'", GAMEDATA_DATABASE_NAME, hwid));
            if(db.execute(query) && db.get_result_row_count() <= 0){
                SQLITE3_QUERY insert_query = SQLITE3_QUERY(fmt::format("INSERT INTO {} VALUES ('{}', '{}', {}, {}, {}, '{}');", GAMEDATA_DATABASE_NAME, hwid, sid, initial_gamestate, remote_player_is_white, 0, initial_fen)); // ? will be replaced after bind
                if (db.execute(insert_query) && db.commit()) { // execute query
                    db.perror();
                    err = true;
                    reason = "db insert error";
                }
            }else{
                SQLITE3_QUERY update_query = SQLITE3_QUERY(fmt::format("UPDATE {} SET SID='{}', game_state='{}', remote_player_is_white='{}', is_syncing_phase='{}', current_board_fen='{}' WHERE HWID='{}';", GAMEDATA_DATABASE_NAME, sid, initial_gamestate, remote_player_is_white, 0, initial_fen, hwid)); // ? will be replaced after bind
                if (db.execute(update_query) && db.commit()) { // execute query
                    db.perror();
                    err = true;
                    reason = "db update error";
                }
            }

        }

        json11::Json response_json = json11::Json::object {
                { "err", err },
                { "status", reason },
                { "sid", "" + sid },
                { "profile", json11::Json::object {{ "hwid", hwid },
                                               { "rank", 0 },
                                               { "elo_rank_readable", "NOOB"},
                                               {"friendly_name", "Player_" + hwid},
                                               {"virtual_player_id", "ID_" + hwid},
                                               {"player_type", playertype}}}
        };
        res.set_content(response_json.dump(), "application/json");
    });

    svr.Get("/rest/heartbeat", [&db](const httplib::Request& req, httplib::Response &res) {
        bool err = false;
        std::string reason = "ok";
        // GET PARAMETER
        const std::string hwid = sanitize_r(req.get_param_value("hwid"));
        const std::string sid = sanitize_r(req.get_param_value("sid"));
        // CHECK INPUT
        if(hwid.empty()){
            err = true;
            reason = "hwid.size() <= 0";
        }else{
            //CHECK SESSION EXISTS
            SQLITE3_QUERY query = SQLITE3_QUERY(fmt::format("SELECT * FROM {} WHERE HWID='{}' AND SID='{}'", GAMEDATA_DATABASE_NAME, hwid, sid));
            if(db.execute(query) && db.get_result_row_count() <= 0) {
                err = true;
                reason = "session invalid, please use login first";
            }
        }

        json11::Json response_json = json11::Json::object {
                { "err", err },
                { "status", reason }
        };
        res.set_content(response_json.dump(), "application/json");
    });

    svr.Get("/rest/set_player_state", [&db](const httplib::Request& req, httplib::Response &res) {
        bool err = false;
        std::string reason = "ok";
        // GET PARAMETER
        const std::string hwid = sanitize_r(req.get_param_value("hwid"));
        const std::string sid = sanitize_r(req.get_param_value("sid"));
        const std::string ps = sanitize_r(req.get_param_value("ps"));
        // CHECK INPUT
        if(hwid.empty()){
            err = true;
            reason = "hwid.size() <= 0";
        }else{

            auto val = magic_enum::enum_cast<GAME_STATE>(std::stoi(ps));
            if (val.has_value()) {
                if(val.value() == GAME_STATE::PS_IDLE || val.value() == GAME_STATE::PS_SEARCHING || val.value() == GAME_STATE::PS_SEARCHING_MANUAL){
                    GAME_STATE gs = val.value();
                    if(gs == GAME_STATE::PS_SEARCHING_MANUAL){
                        gs = GAME_STATE::PS_SEARCHING;
                    }
                    SQLITE3_QUERY update_query = SQLITE3_QUERY(
                            fmt::format("UPDATE {} SET game_state='{}' WHERE HWID='{}' AND SID='{}'",
                                        GAMEDATA_DATABASE_NAME, magic_enum::enum_integer(gs), hwid, sid)); // ? will be replaced after bind
                    if (db.execute(update_query) && db.commit()) { // execute query
                        db.perror();
                        err = true;
                        reason = "db update error";
                    }

                }else{
                    err = true;
                    reason = "invalid ps parameter value";
                }
            }else{
                err = true;
                reason = "invalid ps parameter value";
            }
        }
        json11::Json response_json = json11::Json::object {
                { "err", err },
                { "status", reason }
        };
        res.set_content(response_json.dump(), "application/json");
    });
    // TODO NEXT => SET PLAYERSTATE

    // START WEBSERVER
    const std::string svr_host = ConfigParser::getInstance()->get(ConfigParser::CFG_ENTRY::HTTP_HOST);
    const int svr_port = ConfigParser::getInstance()->getInt_nocheck(ConfigParser::CFG_ENTRY::HTTP_PORT);
    // USING A SEPARATE THREAD
    auto httpThread = std::thread([&]() {svr.listen(svr_host.c_str(), svr_port); });
    main_thread_running = true;

    LOG_F(ERROR, "MAIN LOOP ENTERED");
    while (main_thread_running){

    }
    httpThread.join();

    LOG_F(ERROR, "CLEANUP");


     // WRITE LOGS TO FILESYSTEM
    loguru::flush();
    return 0;
}