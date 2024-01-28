#include <iostream>
#include <cstdio>

#ifdef __MACH__
#include <mach/clock.h>
#include <mach/mach.h>
#endif


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

#include <thc.h>


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

void sys_tick_delay() {
#ifdef __MACH__
    clock_serv_t cclock;
    mach_timespec_t mts;
    host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
    clock_get_time(cclock, &mts);
    mach_port_deallocate(mach_task_self(), cclock);
    t2.tv_sec = mts.tv_sec;
    t2.tv_nsec = mts.tv_nsec;
    // IF TIME DIFFERENCE IS BIG ENOUGHT => CALL THE TIMER EVENT
    if ((t2.tv_sec - t1.tv_sec) > SYSTEM_TICK_DELAY) {
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

enum class AI_ALGORITHMS {
    RNG = 0,
    FIRST = 1
};

enum class GAME_STATE {
    PS_INVALID = 0,
    PS_IDLE = 1,
    PS_SEARCHING = 4,
    PS_SEARCHING_MANUAL = 5,
    PS_PREPARING_INGAME = 6,
    PS_INGAME = 7,
    PS_GAME_OVER = 8
};

struct GAME_DB_ENTRY_READ {
    bool valid = false;
    std::string hwid;
    std::string sid;
    GAME_STATE game_state = GAME_STATE::PS_INVALID;
    bool remote_player_is_white = false;
    bool is_syncing_phase = false;
    std::string current_board_fen;
    bool waiting_for_game = false;
    bool game_running = false;
    bool is_game_over = false;
    std::string game_id;
};


bool update_game_db_entry(SQLITE3 &_db, const std::string &_hwid = "", const std::string &_sid = "",
                          const std::string &_game_state = "", const std::string &_remote_player_is_white = "",
                          const std::string &_is_syncing_phase = "", const std::string &_current_board_fen = "") {

    std::string q = fmt::format("UPDATE {} SET", GAMEDATA_DATABASE_NAME);
    bool updated = false;
    if (!_sid.empty()) {
        q += fmt::format(" SID='{}',", _sid);
        updated = true;
    }

    if (!_game_state.empty()) {
        q += fmt::format(" game_state={},", _game_state);
        updated = true;
    }

    if (!_remote_player_is_white.empty()) {
        q += fmt::format(" remote_player_is_white={},", _remote_player_is_white);
        updated = true;
    }

    if (!_is_syncing_phase.empty()) {
        q += fmt::format(" is_syncing_phase={},", _is_syncing_phase);
        updated = true;
    }

    if (!_current_board_fen.empty()) {
        q += fmt::format(" current_board_fen='{}',", _current_board_fen);
        updated = true;
    }

    // REMOVE FINAL ','
    q.pop_back();


    if (!_hwid.empty()) {
        q += fmt::format(" WHERE HWID={}", _hwid);
    }



    //EXECUTE QUERY
    if (updated) {
        SQLITE3_QUERY update_query = SQLITE3_QUERY(q); // ? will be replaced after bind
        if (_db.execute(update_query) || _db.commit()) { // execute query
            _db.perror();
            return false;
        }
    }
    return true;
}


std::vector<GAME_DB_ENTRY_READ>
get_game_db_entry(SQLITE3 &_db, const std::string &_hwid = "", const std::string &_sid = "") {
    std::vector<GAME_DB_ENTRY_READ> res;
    std::string q = "SELECT game_state, remote_player_is_white, is_syncing_phase, current_board_fen, HWID, SID FROM ?";

    if (!_hwid.empty() || !_sid.empty()) {
        q += "WHERE ";
    }
    if (!_hwid.empty()) {
        q += fmt::format("HWID='{}' ", _hwid);
    }

    if (!_hwid.empty() && !_sid.empty()) {
        q += "AND ";
    }

    if (!_sid.empty()) {
        q += fmt::format("SID='{}' ", _sid);
    }


    SQLITE3_QUERY query = SQLITE3_QUERY(q);
    query.add_binding(GAMEDATA_DATABASE_NAME);
    if (!_db.execute(query) && !_db.get_result_row_count() <= 0) {

#ifdef  DEBUG
        _db.print_result();
#endif

        const auto qres = _db.copy_result();
        for (auto e: *qres) {
            GAME_DB_ENTRY_READ result_entry = GAME_DB_ENTRY_READ();
            result_entry.valid = true;
            result_entry.game_state = magic_enum::enum_cast<GAME_STATE>(std::stoi(e.at(0))).value(); // game_state
            result_entry.remote_player_is_white = std::stoi(e.at(1));
            //     result_entry.is_syncing_phase = std::stoi(e.at(2));
            result_entry.current_board_fen = e.at(3);
            result_entry.hwid = e.at(4);
            result_entry.sid = e.at(5);


            result_entry.game_id = fmt::format("{}-{}", result_entry.hwid, result_entry.sid);

            if (result_entry.game_state == GAME_STATE::PS_SEARCHING ||
                result_entry.game_state == GAME_STATE::PS_SEARCHING_MANUAL) {
                result_entry.waiting_for_game = true;
            } else if (result_entry.game_state == GAME_STATE::PS_INGAME) {
                result_entry.game_running = true;
            } else if (result_entry.game_state == GAME_STATE::PS_GAME_OVER) {
                result_entry.is_game_over = true;
                result_entry.game_running = true;
            } else if (result_entry.game_state == GAME_STATE::PS_PREPARING_INGAME) {
                result_entry.is_syncing_phase = true;
                result_entry.game_running = true;
            }

            res.push_back(result_entry);
        }
    }

    return res;
}

int main(int argc, char *argv[]) {
    // REGISTER SIGNAL HANDLER
    signal(SIGINT, signal_callback_handler);

    //---- PRINT HELP MESSAGE ------------------------------------------------------ //
    if (cmdOptionExists(argv, argv + argc, "--help")) {
        std::cout << "---- ATC_SERVER HELP ----" << std::endl;
        std::cout << "--help                   | prints this message" << std::endl;
        std::cout << "--version                | prints a version message" << std::endl;
        std::cout
                << "--cfgfolderpath          | set the ABS folder path for the config FORMAT: /xyz/folder/ which contains atcserverconfig.ini, if not specified ./atcserverconfig.ini is used"
                << std::endl;
        std::cout << "--statichtmlpath         | set the ABS folder path for the static html files" << std::endl;
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



    // SET SELECTED CHESS ENGINE ALGORITHM
    AI_ALGORITHMS selected_ai_algorithm = AI_ALGORITHMS::RNG;
    std::string chess_ai_algorithm = ConfigParser::getInstance()->get(ConfigParser::CFG_ENTRY::AI_ALGORITHM);
    if (chess_ai_algorithm == "RNG") {
        selected_ai_algorithm = AI_ALGORITHMS::RNG;
    }else if (chess_ai_algorithm == "FIRST") {
        selected_ai_algorithm = AI_ALGORITHMS::FIRST;
    }
    LOG_SCOPE_F(INFO, "USER SELECTED ALGORITHM IS %s ! POSSIBLE ALGORITHMS ARE: RNG, FIRST", chess_ai_algorithm.c_str());


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
    if (!ConfigParser::getInstance()->loadConfigFile(CONFIG_FILE_PATH)) {
        ConfigParser::getInstance()->loadDefaults("");
        ConfigParser::getInstance()->createConfigFile(CONFIG_FILE_PATH, true);
        LOG_F(INFO, "WRITE NEW CONFIGFILE DUE MISSING ONE: atcserverconfig.ini");
    }
    LOG_F(INFO, "CONFIG FILE LOADED");


    //LOAD DATABASE
    const std::string dbname = ConfigParser::getInstance()->get(ConfigParser::CFG_ENTRY::DB_FILEPATH);
    SQLITE3 db; // init database
    db.open(dbname);

    //INIT DATABASE
    if (cmdOptionExists(argv, argv + argc, "--reset")) {
        db.execute(fmt::format("DROP TABLE IF EXISTS {};", GAMEDATA_DATABASE_NAME).c_str());
        db.commit();
        LOG_F(INFO, "GAME DATA TABLE DROPPED DUE SET --reset FLAG");
    }

    // CREATE DATA TABLE IF NOT EXISTS
    if (db.execute(fmt::format(
            "CREATE TABLE IF NOT EXISTS {} (HWID TEXT NOT NULL UNIQUE,SID TEXT, game_state INTEGER,remote_player_is_white INTEGER, is_syncing_phase INTEGER,current_board_fen TEXT, PRIMARY KEY(HWID));",
            GAMEDATA_DATABASE_NAME).c_str())) {
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
        //      LOG_F(INFO, "%s", fmt::format("req: {} res {}", req.body, res.body).c_str());
    });
    //SET ERROR HANDLER
    svr.set_error_handler([](const httplib::Request & /*req*/, httplib::Response &res) {
        const char *fmt = "<p>Error Status: <span style='color:red;'>%d</span></p>";
        char buf[BUFSIZ];
        snprintf(buf, sizeof(buf), fmt, res.status);
        res.set_content(buf, "text/html");
    });

    // SERVER STATIC FILES
    std::string static_html_path = ConfigParser::getInstance()->get(ConfigParser::CFG_ENTRY::STATIC_HTML_PATH);
    if (cmdOptionExists(argv, argv + argc, "--statichtmlpath")) {
        static_html_path = std::string(getCmdOption(argv, argv + argc, "--statichtmlpath"));
        LOG_F(ERROR, "set STATIC_HTML_PATH using value of --statichtmlpath parameter");
    }
    auto ret = svr.set_mount_point("/public", static_html_path.c_str());
    if (!ret) {
        LOG_F(ERROR, "set STATIC_HTML_PATH path cant be accessed");
    }

    // REGISTER ROUTES
    svr.Get("/", [](const httplib::Request &req, httplib::Response &res) {
        res.set_redirect("/public/index.html");
    });

    svr.Get("/rest/logout", [&db](const httplib::Request &req, httplib::Response &res) {
        bool err = false;
        std::string reason = "ok";
        // RESET PLAYER STATE
        const std::string hwid = sanitize_r(req.get_param_value("hwid"));

        if (!update_game_db_entry(db, hwid, "", fmt::format("{}", magic_enum::enum_integer(GAME_STATE::PS_IDLE)),
                                  "", "", "")) { // execute query
            db.perror();
            err = true;
            reason = "db update error";
        }

        json11::Json response_json = json11::Json::object{
                {"err",    err},
                {"status", reason},
        };
        res.set_content(response_json.dump(), "application/json");
    });

    svr.Get("/rest/login", [&db](const httplib::Request &req, httplib::Response &res) {

        bool err = false;
        std::string reason = "ok";

        const std::string hwid = sanitize_r(req.get_param_value("hwid"));
        const std::string playertype = sanitize_r(req.get_param_value("playertype"));

        //GENERATE SID :)
        std::string sid = hwid;
        std::reverse(sid.begin(), sid.end());

        // CHECK INPUT
        if (hwid.empty()) {
            err = true;
            reason = "hwid.size() <= 0";
        } else {


            const int initial_gamestate = magic_enum::enum_integer(GAME_STATE::PS_IDLE);
            const int remote_player_is_white = randomBoolean() * 1;
            std::string initial_fen = ConfigParser::getInstance()->get(
                    ConfigParser::CFG_ENTRY::INITIAL_BOARD_FEN);

            if (initial_fen.empty()) {
                thc::ChessRules cr;
                initial_fen = cr.ForsythPublish();
            }
            //CREATE NEW USER IF NOT EXISTS
            SQLITE3_QUERY query = SQLITE3_QUERY("SELECT * FROM ? WHERE HWID=?");
            query.add_binding(GAMEDATA_DATABASE_NAME, hwid);

            if (db.execute(query)) {
                db.perror();
                err = true;
                reason = "db select * error";
            } else {

                const int r = db.copy_result()->size();
                db.print_result();
                db.commit();
                query.reset_binding();
                if (r <= 0) {
                    query.set_query_template(
                            "INSERT INTO ? VALUES (?, ?, ?, ?, ?, ?)"); // ? will be replaced after bind
                    query.add_binding(GAMEDATA_DATABASE_NAME,
                                      hwid, sid, fmt::format("{}", initial_gamestate),
                                      fmt::format("{}", remote_player_is_white), fmt::format("{}", 0),
                                      initial_fen);
                    if (db.execute(query) || db.commit()) { // execute query adn commit new data
                        db.perror();
                        err = true;
                        reason = "db insert error";
                    }

                } else {
                    query.set_query_template(
                            "UPDATE ? SET SID=?, game_state=?, remote_player_is_white=?, is_syncing_phase=?, current_board_fen=? WHERE HWID=?"); // ? will be replaced after bind
                    query.add_binding(GAMEDATA_DATABASE_NAME, sid, fmt::format("{}", initial_gamestate),
                                      fmt::format("{}", remote_player_is_white), fmt::format("{}", 0),
                                      initial_fen, hwid);
                    if (db.execute(query) || db.commit()) { // execute query
                        db.perror();
                        err = true;
                        reason = "db update error";
                    }
                }
            }
        }

        json11::Json response_json = json11::Json::object{
                {"err",     err},
                {"status",  reason},
                {"sid",     "" + sid},
                {"profile", json11::Json::object{{"hwid",              hwid},
                                                 {"rank",              0},
                                                 {"elo_rank_readable", "NOOB"},
                                                 {"friendly_name",     "Player_" + hwid},
                                                 {"virtual_player_id", "ID_" + hwid},
                                                 {"player_type",       playertype}}}
        };

        res.set_content(response_json.dump(), "application/json");
    });

    svr.Get("/rest/heartbeat", [&db](const httplib::Request &req, httplib::Response &res) {
        bool err = false;
        std::string reason = "ok";
        // GET PARAMETER
        const std::string hwid = sanitize_r(req.get_param_value("hwid"));
        const std::string sid = sanitize_r(req.get_param_value("sid"));
        // CHECK INPUT
        if (hwid.empty()) {
            err = true;
            reason = "hwid.size() <= 0";
        } else {
            //CHECK SESSION EXISTS
            if (get_game_db_entry(db, hwid, sid).empty()) {
                err = true;
                reason = "session invalid, please use login first";
            }
        }

        json11::Json response_json = json11::Json::object{
                {"err",    err},
                {"status", reason}
        };
        res.set_content(response_json.dump(), "application/json");
    });

    svr.Get("/rest/set_player_state", [&db](const httplib::Request &req, httplib::Response &res) {

        bool err = false;
        std::string reason = "ok";
        // GET PARAMETER
        const std::string hwid = sanitize_r(req.get_param_value("hwid"));
        const std::string sid = sanitize_r(req.get_param_value("sid"));
        const std::string ps = sanitize_r(req.get_param_value("ps"));
        // CHECK INPUT
        if (hwid.empty()) {
            err = true;
            reason = "hwid.size() <= 0";
        } else {

            auto val = magic_enum::enum_cast<GAME_STATE>(std::stoi(ps));
            if (val.has_value()) {
                if (val.value() == GAME_STATE::PS_IDLE || val.value() == GAME_STATE::PS_SEARCHING ||
                    val.value() == GAME_STATE::PS_SEARCHING_MANUAL) {
                    GAME_STATE gs = val.value();
                    if (gs == GAME_STATE::PS_SEARCHING_MANUAL) {
                        gs = GAME_STATE::PS_SEARCHING;
                    }


                    SQLITE3_QUERY update_query = SQLITE3_QUERY(
                            "UPDATE ? SET game_state=? WHERE HWID=? AND SID=?"); // ? will be replaced after bind
                    update_query.add_binding(GAMEDATA_DATABASE_NAME, fmt::format("{}", magic_enum::enum_integer(gs)),
                                             hwid, sid);
                    if (db.execute(update_query) || db.commit()) { // execute query
                        db.perror();
                        err = true;
                        reason = "db update error";
                    }

                } else {
                    err = true;
                    reason = "invalid ps parameter value";
                }
            } else {
                err = true;
                reason = "invalid ps parameter value";
            }
        }
        json11::Json response_json = json11::Json::object{
                {"err",    err},
                {"status", reason}
        };
        res.set_content(response_json.dump(), "application/json");
    });

    svr.Get("/rest/abort_game", [&db](const httplib::Request &req, httplib::Response &res) {
        bool err = false;
        std::string reason = "ok";
        // GET PARAMETER
        const std::string hwid = sanitize_r(req.get_param_value("hwid"));
        const std::string sid = sanitize_r(req.get_param_value("sid"));
        // CHECK INPUT
        if (hwid.empty()) {
            err = true;
            reason = "hwid.size() <= 0";
        } else {
            if (!update_game_db_entry(db, hwid, "", fmt::format("{}", magic_enum::enum_integer(GAME_STATE::PS_IDLE)),
                                      "", "", "")) { // execute query
                db.perror();
                err = true;
                reason = "db update error";
            }
        }
        json11::Json response_json = json11::Json::object{
                {"err",    err},
                {"status", reason}
        };
        res.set_content(response_json.dump(), "application/json");
    });

    svr.Get("/rest/set_user_config", [&db](const httplib::Request &req, httplib::Response &res) {
        json11::Json response_json = json11::Json::object{
                {"err",    false},
                {"status", "ok"},
        };
        res.set_content(response_json.dump(), "application/json");
    });

    svr.Get("/rest/store_user_log", [&db](const httplib::Request &req, httplib::Response &res) {
        json11::Json response_json = json11::Json::object{
                {"err",    false},
                {"status", "ok"},
        };
        res.set_content(response_json.dump(), "application/json");
    });

    svr.Get("/rest/client_status", [&db](const httplib::Request &req, httplib::Response &res) {
        json11::Json response_json = json11::Json::object{
                {"err",    false},
                {"status", "ok"},
        };
        res.set_content(response_json.dump(), "application/json");
    });

    svr.Get("/rest/player_setup_confirmation", [&db](const httplib::Request &req, httplib::Response &res) {
        bool err = false;
        std::string reason = "ok";
        // GET PARAMETER
        const std::string hwid = sanitize_r(req.get_param_value("hwid"));
        const std::string sid = sanitize_r(req.get_param_value("sid"));

        // CHECK INPUT
        if (hwid.empty()) {
            err = true;
            reason = "hwid.size() <= 0";
        } else {
            auto qres = get_game_db_entry(db, hwid, sid);

            if (qres.empty()) {
                err = true;
                reason = "session invalid, please use login first";
            } else {

                if (qres.at(0).is_syncing_phase) {
                    update_game_db_entry(db, hwid, "",
                                         fmt::format("{}", magic_enum::enum_integer(GAME_STATE::PS_INGAME)), "",
                                         "", "");
                }
            }
        }
        json11::Json response_json = json11::Json::object{
                {"err",    err},
                {"status", reason}
        };
        res.set_content(response_json.dump(), "application/json");
    });

    svr.Get("/rest/make_move", [&db](const httplib::Request &req, httplib::Response &res) {
        bool err = false;
        std::string reason = "ok";
        // GET PARAMETER
        const std::string hwid = sanitize_r(req.get_param_value("hwid"));
        const std::string sid = sanitize_r(req.get_param_value("sid"));
        const std::string move = sanitize_r(req.get_param_value("move"));

        // CHECK INPUT
        if (hwid.empty()) {
            err = true;
            reason = "hwid.size() <= 0";
        } else if (move.empty()) {
            err = true;
            reason = "move.size() <= 0";
        } else {
            auto qres = get_game_db_entry(db, hwid, sid);

            if (qres.empty()) {
                err = true;
                reason = "session invalid, please use login first";
            } else {

                auto qres = get_game_db_entry(db, hwid, sid);

                if (qres.empty()) {
                    err = true;
                    reason = "session invalid, please use login first";
                } else if (!qres.at(0).game_running) {
                    err = true;
                    reason = "game not running";
                } else if (qres.at(0).waiting_for_game) {
                    err = true;
                    reason = "waiting_for_game = game not running";
                } else {
                    // QUERY CURRENT GAME STATE FROM DATABASE
                    const GAME_DB_ENTRY_READ gdbe = qres.at(0);

                    thc::ChessRules cr;
                    if (cr.Forsyth(gdbe.current_board_fen.c_str())) {
                        if ((cr.WhiteToPlay() && gdbe.remote_player_is_white) ||
                            (!cr.WhiteToPlay() && !gdbe.remote_player_is_white)) {

                            //APPLY MOVE
                            thc::Move mv;
                            if (mv.TerseIn(&cr, move.c_str()) && mv.Valid()) {
                                cr.PlayMove(mv);

                                // UPDATE FEN IN DATABASE ENTRY
                                update_game_db_entry(db, gdbe.hwid, "", "", "", "", cr.ForsythPublish());

                            } else {
                                err = true;
                                reason = "move invalid";
                            }
                        } else {
                            err = true;
                            reason = "other players turn";
                        }
                    } else {
                        err = true;
                        reason = "fen not valid";
                    }

                }


            }
        }
        json11::Json response_json = json11::Json::object{
                {"err",    err},
                {"status", reason}
        };
        res.set_content(response_json.dump(), "application/json");
    });

    svr.Get("/rest/get_player_state", [&db](const httplib::Request &req, httplib::Response &res) {

                bool err = false;
                std::string reason = "ok";
                // GET PARAMETER
                const std::string hwid = sanitize_r(req.get_param_value("hwid"));
                const std::string sid = sanitize_r(req.get_param_value("sid"));
                const std::string simplified = sanitize_r(req.get_param_value("simplified"));


                // MATCHMAKING VARIABLES
                bool waiting_for_game = false;
                GAME_STATE ps = GAME_STATE::PS_IDLE;
                // GAMESTATE VARIABLES
                bool game_running = false;
                bool im_white_player = false;
                bool is_my_turn = false;
                bool is_syncing_phase = false;

                std::string initial_board = ConfigParser::getInstance()->get(ConfigParser::CFG_ENTRY::INITIAL_BOARD_FEN);
                if (initial_board.empty()) {
                    thc::ChessRules cr;
                    initial_board = cr.ForsythPublish();
                }

                std::string game_id;
                std::string current_board_fen = initial_board;
                std::string current_board_fen_simple;
                bool is_board_valid = false;
                bool is_game_over = false;
                std::string move = ""; // ?
                std::vector<std::string> legal_moves;


                // CHECK INPUT
                if (hwid.empty()) {
                    err = true;
                    reason = "hwid.size() <= 0";
                } else {

                    auto qres = get_game_db_entry(db, hwid, sid);

                    if (qres.empty()) {
                        err = true;
                        reason = "session invalid, please use login first";
                    } else {
                        // QUERY CURRENT GAME STATE FROM DATABASE
                        const GAME_DB_ENTRY_READ gdbe = qres.at(0);
                        waiting_for_game = gdbe.waiting_for_game;
                        game_running = gdbe.game_running;
                        is_game_over = gdbe.is_game_over;
                        im_white_player = gdbe.remote_player_is_white;
                        is_syncing_phase = gdbe.is_syncing_phase;
                        current_board_fen = gdbe.current_board_fen;
                        game_id = gdbe.game_id;
                        // PARSE CURRENT BOARD FEN AND DETERM CURRENT BOARD STATUS
                        thc::ChessRules cr;
                        is_board_valid = cr.Forsyth(gdbe.current_board_fen.c_str());
                        current_board_fen = cr.ForsythPublish();
                        cr.WhiteToPlay();

                        if ((cr.WhiteToPlay() && im_white_player) || (!cr.WhiteToPlay() && !im_white_player)) {
                            is_my_turn = true;
                        } else {
                            is_my_turn = false;
                        }
                        std::vector<thc::Move> moves;
                        cr.GenLegalMoveList(moves);

                        for (auto m: moves) {
                            std::string mvs = m.TerseOut();
                            //strip figure type e4f4q => e4f4
                            if (mvs.length() > 4) {
                                mvs.pop_back();
                            }
                            legal_moves.push_back(mvs);
                        }

                        //EXTRACT SIMPLIFIED FEN WITHOUT EXTRAC INFORMATION NEEDED FOR THE CHESS TABLE PARSING ALGORITHM
                        if (cr.WhiteToPlay()) {
                            current_board_fen_simple = current_board_fen.substr(0, current_board_fen.find(" w "));
                        } else {
                            current_board_fen_simple = current_board_fen.substr(0, current_board_fen.find(" b "));
                        }
                    }
                }


                json11::Json response_json = json11::Json::object{
                        {"err",               err},
                        {"status",            reason},
                        {"matchmaking_state", json11::Json::object{{"waiting_for_game", waiting_for_game},
                                                                   {"detailed",         json11::Json::object{
                                                                           {"player_state",
                                                                            magic_enum::enum_integer(ps)}}}}},
                        {"game_state",        json11::Json::object{{"game_running", game_running},
                                                                   {"detailed",     json11::Json::object{{"id", game_id}}},

                                                                   {"simplified",   json11::Json::object{{"im_white_player",  im_white_player},
                                                                                                         {"is_my_turn",       is_my_turn},
                                                                                                         {"is_syncing_phase", is_syncing_phase},
                                                                                                         {"current_board",    json11::Json::object{{"fen",            current_board_fen_simple},
                                                                                                                                                   {"ExtendetFen",    current_board_fen},
                                                                                                                                                   {"initial_board",  initial_board},
                                                                                                                                                   {"is_board_valid", is_board_valid},
                                                                                                                                                   {"is_game_over",   is_game_over},
                                                                                                                                                   {"move",           move},
                                                                                                                                                   {"legal_moves",    legal_moves}}}}}}}
                };
                res.set_content(response_json.dump(), "application/json");
            }
    );



// PLAYER STATE


// START WEBSERVER
    const std::string svr_host = ConfigParser::getInstance()->get(ConfigParser::CFG_ENTRY::HTTP_HOST);
    const int svr_port = ConfigParser::getInstance()->getInt_nocheck(ConfigParser::CFG_ENTRY::HTTP_PORT);
// USING A SEPARATE THREAD
    auto httpThread = std::thread([&]() { svr.listen(svr_host.c_str(), svr_port); });


    main_thread_running = true;

    LOG_F(ERROR, "MAIN LOOP ENTERED");
    while (main_thread_running) {
        auto qres = get_game_db_entry(db);
        for (
            auto r
                : qres) {


            if (r.waiting_for_game) {
                // SWITCHING TO PREPARE FOR GAME
                update_game_db_entry(db, r.hwid, "",
                                     fmt::format("{}", magic_enum::enum_integer(GAME_STATE::PS_PREPARING_INGAME)), "",
                                     "", "");
            } else if (r.is_syncing_phase) {

            } else if (r.is_game_over) {
                update_game_db_entry(db, r.hwid, "",
                                     fmt::format("{}", magic_enum::enum_integer(GAME_STATE::PS_IDLE)), "",
                                     "", "");
            } else if (r.game_running) {


                // CHECK IF AIs TURN

                thc::ChessRules cr;
                if (cr.Forsyth(r.current_board_fen.c_str())) {


                    //GENERATE LEGAL MOVES
                    std::vector<thc::Move> moves;
                    cr.GenLegalMoveList(moves);


                    if (moves.empty()) {
                        update_game_db_entry(db, r.hwid, "",
                                             fmt::format("{}", magic_enum::enum_integer(GAME_STATE::PS_GAME_OVER)), "",
                                             "", "");



                        // IS AI TURN
                    } else if ((!cr.WhiteToPlay() && r.remote_player_is_white) ||
                               (cr.WhiteToPlay() && !r.remote_player_is_white)) {


                        thc::Move move_to_apply_by_ai;

                        if (selected_ai_algorithm == AI_ALGORITHMS::RNG) {
                            std::random_device rd; // obtain a random number from hardware
                            std::mt19937 gen(rd()); // seed the generator
                            if (!moves.empty()) {
                                std::uniform_int_distribution<> distr(0, moves.size()-1); // define the range
                                const int selected_move = distr(gen);
                                move_to_apply_by_ai = moves.at(selected_move);
                            }
                        }else if (selected_ai_algorithm == AI_ALGORITHMS::FIRST) {
                            if (!moves.empty()) {
                                move_to_apply_by_ai = moves.at(0);
                            }
                        }





                        if (move_to_apply_by_ai.Valid()) {
                            cr.PlayMove(move_to_apply_by_ai);
                            // UPDATE FEN IN DATABASE ENTRY
                            update_game_db_entry(db, r.hwid, "", "", "", "", cr.ForsythPublish());
                        }
                    }
                }
            }


            // RESET GAME IF THERE IS ANY FEN ERROR
            thc::ChessRules cr;
            if (!cr.Forsyth(r.current_board_fen.c_str())) {
                update_game_db_entry(db, r.hwid, "",
                                     fmt::format("{}", magic_enum::enum_integer(GAME_STATE::PS_IDLE)), "",
                                     "", "");
            }


        }
    }
    httpThread.

            join();

    LOG_F(ERROR, "CLEANUP");


// WRITE LOGS TO FILESYSTEM
    loguru::flush();

    return 0;
}

