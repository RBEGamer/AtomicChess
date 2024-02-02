//
// Created by marcel on 02.02.24.
//

#ifndef ATC_SERVER_DB_ADAPTER_H
#define ATC_SERVER_DB_ADAPTER_H

#include <fmt/core.h>

#include "SHARED/loguru-master/loguru.hpp"
#include "SHARED/magic_enum-master/include/magic_enum.hpp"

#include "SQLITE3_QUERY.hpp"
#include "SQLITE3.hpp"

#include "game_state.h"

#define SQLITE_DATABASE_FILE_EXTENTION ".sqlite"
#define SQLITE_DATABASE_GAME_TABLE_NAME "atcgamedata"


#define DBC_HWID "hwid"
#define DBC_SID "sid"
#define DBC_GS "game_state"
#define DBC_RPISW "remote_player_is_white"
#define DBC_OPH "other_player_hwid"
#define DBC_CBF "current_board_fen"

class db_adapter {


public:

    enum class GAME_DB_ENTRY_WRITE_UPDATE_ATTRIBUTES{
        ALL = 0,
        SID = 1 << 0,
        STATE = 1 << 1,
        REMOTE_PLAYER_IS_WHITE = 1 << 2,
        OTHER_PLAYER_HWID = 1 << 3,
        CURRENT_BOARD_FE = 1 << 4,
    };
    using GAME_DB_ENTRY_WRITE_UPDATE_ATTRIBUTES_TYPE = std::underlying_type<GAME_DB_ENTRY_WRITE_UPDATE_ATTRIBUTES>::type;



    struct GAME_DB_ENTRY_READ {
        bool valid = false;
        std::string hwid;
        std::string sid;
        game_state::GAME_STATE state = game_state::GAME_STATE::PS_INVALID;
        bool remote_player_is_white = false;
        bool is_syncing_phase = false;
        std::string current_board_fen;
        std::string other_player_hwid;
        bool waiting_for_game = false;
        bool game_running = false;
        bool is_game_over = false;
        std::string game_id;
    };

    struct GAME_DB_ENTRY_WRITE {
        std::string hwid;
        std::string sid;
        game_state::GAME_STATE state = game_state::GAME_STATE::PS_INVALID;
        bool remote_player_is_white = false;
        std::string current_board_fen;
        std::string other_player_hwid;

        GAME_DB_ENTRY_WRITE()= default;

        GAME_DB_ENTRY_WRITE(const std::string _hwid, const std::string _sid, const std::string _fen){
            hwid = _hwid;
            sid = _sid;
            current_board_fen = _fen;
        }
    };



    db_adapter(const std::string& _db_filename, const std::string& _game_table_name, bool _clear_existing_database);
    bool open(const std::string& _db_filename);
    std::vector<db_adapter::GAME_DB_ENTRY_READ> get_game_db_entry(const std::string &_hwid = "", const std::string &_sid = "");
    bool insert_game_db_entry(const GAME_DB_ENTRY_WRITE _entry);
    bool update_game_db_entry(const GAME_DB_ENTRY_WRITE _entry, const GAME_DB_ENTRY_WRITE_UPDATE_ATTRIBUTES _attr_to_update = GAME_DB_ENTRY_WRITE_UPDATE_ATTRIBUTES::ALL);
    ~db_adapter();

private:
    bool init_ok = false;
    SQLITE3 db;
    std::string table_name = SQLITE_DATABASE_GAME_TABLE_NAME;
    std::string database_path = SQLITE_DATABASE_FILE_EXTENTION;
    bool create_table(bool _clear_if_existing);
};


#endif //ATC_SERVER_DB_ADAPTER_H
