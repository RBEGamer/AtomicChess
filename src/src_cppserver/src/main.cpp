#include <iostream>
#include <sqlite3.h>
#include <format>

#include "SHARED/loguru-master/loguru.hpp"
#include <fmt/core.h>

#include "helper.h"
#include "SQLITE3_QUERY.hpp"
#include "SQLITE3.hpp"
#include "ConfigParser.h"

#define LOG_FILE_PATH_ERROR "./atcserver_error_log.log"
#define LOG_FILE_PATH "./atcserver_log.log"
#define DEFAULT_CONFIG_FILE_PATH "./atcserverconfig.ini"
#define GAMEDATA_DATABASE_NAME "gamedata"

#ifndef VERSION
    #define VERSION "1.0.0"
#endif
void signal_callback_handler(int signum) {
    printf("Caught signal %d\n", signum);

    LOG_F(ERROR, "Caught signal %d\n", signum);
    loguru::flush();

    exit(signum);
}


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
   // std::cout << "Table test created" << std::endl;


    // run complex query using SQLITE3_QUERY object
   /*
    SQLITE3_QUERY query = SQLITE3_QUERY("INSERT INTO test VALUES (?, ?),(?, ?);"); // ? will be replaced after bind
    query.add_binding("100", "foo", "200", "bar"); // add binding values
    if (db.execute(query)) { // execute query
        db.perror();
    }
    std::cout << "2 row inserted to table test" << std::endl;
    if (db.execute(query)) { // error because we execute the same insert query
        db.perror(); // get error message
        std::cerr.flush();
    }
    std::cout << "Repeated insertion failed" << std::endl;


    // methods from SQLITE3_QUERY can be chained, however, no chained function call can be after add_binding
    query.set_query_template("SELECT * FROM ?;").reset_binding().add_binding("test");
    if(!db.execute(query)){
        db.print_result();
    } else {
        db.perror();
    }


    // get number of rows returned
    std::cout << db.get_result_row_count() << " rows returned" << std::endl;


    // get number of cols returned
    std::cout << db.get_result_col_count() << " cols returned" << std::endl;


    // get column names
    auto column_name = db.copy_column_names();
    std::cout << "|";
    for (auto &name : *column_name) {
        std::cout << name << "|";
    }
    std::cout << std::endl;


    // result is stored in a vector of SQLITE_ROW_VECTOR
    auto result = db.copy_result();
    std::cout << "The first element of first row is: " << result->at(0).at(0) << std::endl;


    // commit to save changes
    db.commit();
    std::cout << "Changes committed" << std::endl;


    // add user defined function to database
    db.add_function("PrintHello", //name of function
                    1, // number of argument the UDF take
                    [](sqlite3_context* c, int argc, sqlite3_value** value){ // function implementation
                        int id = sqlite3_value_int(value[0]);
                        std::string result = std::string("Hello" + std::to_string(id));

                        sqlite3_result_text(c, result.c_str(), result.length(), nullptr);
                    });
    db.execute("SELECT PrintHello(id) FROM TEST;");
    db.print_result();


    // drop table
   // db.execute("DROP TABLE test;");
    //std::cout << "Table test dropped" << std::endl;
    //db.commit();
*/
    return 0;
}