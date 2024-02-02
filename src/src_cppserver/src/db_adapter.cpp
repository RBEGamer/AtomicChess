//
// Created by marcel on 02.02.24.
//

#include "db_adapter.h"

db_adapter::db_adapter(const std::string& _db_filename, const std::string& _game_table_name,
                       const bool _clear_existing_database) {


    std::string fn = _db_filename;
    std::string gtn = _game_table_name;

    if(fn.empty()){
        LOG_F(ERROR, "_db_filename paraneter empty. using defaults");
        fn = "./database.sqlite";
    }

    if(gtn.empty()){
        LOG_F(ERROR, "_game_table_name paraneter empty. using defaults");
        gtn = SQLITE_DATABASE_GAME_TABLE_NAME;
    }


    this->table_name = gtn;
    this->database_path = fn;


    this->init_ok = open(fn);
    this->init_ok = create_table(_clear_existing_database);



}

bool db_adapter::create_table(bool _clear_if_existing) {
    if(!init_ok){
        LOG_F(ERROR, "databse not opened, please call open");
        return false;
    }
    // DELETE DATABASE IF FLAG SET
    if(_clear_if_existing){
        if(db.execute(fmt::format("DROP TABLE IF EXISTS {};", this->table_name).c_str()) || db.commit()){
            this->db.perror();
            return false;
        }
    }

    const std::string qs  = fmt::format("CREATE TABLE IF NOT EXISTS {0} ({1} TEXT NOT NULL UNIQUE, {2} TEXT, {3} TEXT, {4} TEXT, {5} TEXT, {6} TEXT, PRIMARY KEY({1}));",this->table_name, DBC_HWID, DBC_SID, DBC_GS, DBC_RPISW, DBC_OPH, DBC_CBF);
    // CREATE TABLE
    if (db.execute(qs.c_str())) {
        db.perror(); // in case of error, perror will print detail to stderr
        return false;
    }
    db.commit();
    return true;
}

std::vector<db_adapter::GAME_DB_ENTRY_READ> db_adapter::get_game_db_entry(const std::string &_hwid, const std::string &_sid) {

    std::vector<GAME_DB_ENTRY_READ> res;

    if(!init_ok){
        LOG_F(ERROR, "databse not opened, please call open");
        return res;
    }

    std::string q = fmt::format("SELECT {}, {}, {}, {}, {}, {} FROM {}", DBC_GS, DBC_RPISW, DBC_OPH, DBC_CBF, DBC_HWID, DBC_SID, this->table_name);

    if (!_hwid.empty() || !_sid.empty()) {
        q += "WHERE ";
    }
    if (!_hwid.empty()) {
        q += fmt::format("{}='{}' ", DBC_HWID, _hwid);
    }

    if (!_hwid.empty() && !_sid.empty()) {
        q += "AND ";
    }

    if (!_sid.empty()) {
        q += fmt::format("{}='{}' ", DBC_SID, _sid);
    }


    SQLITE3_QUERY query = SQLITE3_QUERY(q);
    if(this->db.execute(query)){
        this->db.perror();
        return res;
    }

    if(this->db.get_result_row_count() <= 0){
        return res;
    }



#ifdef  DEBUG
        _db.print_result();
#endif

        const auto qres = this->db.copy_result();
        for (auto e: *qres) {
            GAME_DB_ENTRY_READ result_entry = GAME_DB_ENTRY_READ();
            result_entry.valid = true;
            result_entry.state = magic_enum::enum_cast<game_state::GAME_STATE>(std::stoi(e.at(0))).value(); // game_state
            result_entry.remote_player_is_white = std::stoi(e.at(1));
            result_entry.other_player_hwid = e.at(2);
            result_entry.current_board_fen = e.at(3);
            result_entry.hwid = e.at(4);
            result_entry.sid = e.at(5);


            result_entry.game_id = fmt::format("{}-{}", result_entry.hwid, result_entry.sid);

            if (result_entry.state == game_state::GAME_STATE::PS_SEARCHING ||
                result_entry.state == game_state::GAME_STATE::PS_SEARCHING_MANUAL) {
                result_entry.waiting_for_game = true;
            } else if (result_entry.state == game_state::GAME_STATE::PS_INGAME) {
                result_entry.game_running = true;
            } else if (result_entry.state == game_state::GAME_STATE::PS_GAME_OVER) {
                result_entry.is_game_over = true;
                result_entry.game_running = true;
            } else if (result_entry.state == game_state::GAME_STATE::PS_PREPARING_INGAME) {
                result_entry.is_syncing_phase = true;
                result_entry.game_running = true;
            }

            res.push_back(result_entry);
        }


    return res;
}

db_adapter::~db_adapter() {

}

bool db_adapter::open(const std::string &_db_filename) {

    std::string fn = _db_filename;
    if(fn.find(SQLITE_DATABASE_FILE_EXTENTION) == std::string::npos){
        fn += SQLITE_DATABASE_FILE_EXTENTION;
    }

    if(this->db.open(_db_filename)){
        LOG_F(ERROR, "db open error:");
        this->db.perror();
        return false;
    }

    return true;
}

bool db_adapter::insert_game_db_entry(const db_adapter::GAME_DB_ENTRY_WRITE _entry) {
    if(!init_ok){
        LOG_F(ERROR, "databse not opened, please call open");
        return false;
    }

    const int initial_gamestate = magic_enum::enum_integer(_entry.state);
    const std::string q = fmt::format("INSERT INTO {} VALUES (?, ?, ?, ?, ?, ?)", this->table_name, _entry.hwid, _entry.sid, initial_gamestate, _entry.remote_player_is_white, _entry.other_player_hwid, _entry.current_board_fen);

    SQLITE3_QUERY query(q);

    if (this->db.execute(query) || this->db.commit()) { // execute query adn commit new data-
        db.perror();
        return false;
    }
    return true;
}

bool db_adapter::update_game_db_entry(const GAME_DB_ENTRY_WRITE _entry, const GAME_DB_ENTRY_WRITE_UPDATE_ATTRIBUTES _attr_to_update) {
    if(!init_ok){
        LOG_F(ERROR, "databse not opened, please call open");
        return false;
    }
    bool updated = false;
    if(_entry.hwid.empty()){
        LOG_F(ERROR, "GAME_DB_ENTRY_WRITE::hwid field is empty");
        return false;
    }

    std::string q = fmt::format("UPDATE {} SET", this->table_name);





    if (static_cast<bool>(static_cast<GAME_DB_ENTRY_WRITE_UPDATE_ATTRIBUTES_TYPE >(_attr_to_update) & static_cast<GAME_DB_ENTRY_WRITE_UPDATE_ATTRIBUTES_TYPE>(GAME_DB_ENTRY_WRITE_UPDATE_ATTRIBUTES::SID)) || _attr_to_update == GAME_DB_ENTRY_WRITE_UPDATE_ATTRIBUTES::ALL) {
        q += fmt::format(" {}={},", DBC_SID, _entry.sid);
        updated = true;
    }

    if (static_cast<bool>(static_cast<GAME_DB_ENTRY_WRITE_UPDATE_ATTRIBUTES_TYPE >(_attr_to_update) & static_cast<GAME_DB_ENTRY_WRITE_UPDATE_ATTRIBUTES_TYPE>(GAME_DB_ENTRY_WRITE_UPDATE_ATTRIBUTES::STATE)) || _attr_to_update == GAME_DB_ENTRY_WRITE_UPDATE_ATTRIBUTES::ALL) {
        const int gs = magic_enum::enum_integer(_entry.state);
        q += fmt::format(" {}={},", DBC_GS, gs);
        updated = true;
    }

    if (static_cast<bool>(static_cast<GAME_DB_ENTRY_WRITE_UPDATE_ATTRIBUTES_TYPE >(_attr_to_update) & static_cast<GAME_DB_ENTRY_WRITE_UPDATE_ATTRIBUTES_TYPE>(GAME_DB_ENTRY_WRITE_UPDATE_ATTRIBUTES::REMOTE_PLAYER_IS_WHITE)) || _attr_to_update == GAME_DB_ENTRY_WRITE_UPDATE_ATTRIBUTES::ALL) {
        q += fmt::format(" {}={},", DBC_RPISW, _entry.remote_player_is_white);
        updated = true;
    }

    if (static_cast<bool>(static_cast<GAME_DB_ENTRY_WRITE_UPDATE_ATTRIBUTES_TYPE >(_attr_to_update) & static_cast<GAME_DB_ENTRY_WRITE_UPDATE_ATTRIBUTES_TYPE>(GAME_DB_ENTRY_WRITE_UPDATE_ATTRIBUTES::OTHER_PLAYER_HWID)) || _attr_to_update == GAME_DB_ENTRY_WRITE_UPDATE_ATTRIBUTES::ALL) {
        q += fmt::format(" {}={},", DBC_OPH, _entry.other_player_hwid);
        updated = true;
    }

    if (static_cast<bool>(static_cast<GAME_DB_ENTRY_WRITE_UPDATE_ATTRIBUTES_TYPE >(_attr_to_update) & static_cast<GAME_DB_ENTRY_WRITE_UPDATE_ATTRIBUTES_TYPE>(GAME_DB_ENTRY_WRITE_UPDATE_ATTRIBUTES::CURRENT_BOARD_FE)) || _attr_to_update == GAME_DB_ENTRY_WRITE_UPDATE_ATTRIBUTES::ALL) {
        q += fmt::format(" {}={},", DBC_OPH, _entry.current_board_fen);
        updated = true;
    }


    // REMOVE FINAL ','
    q.pop_back();

    //ADD HWID
    q += fmt::format(" WHERE {}='{}'", DBC_HWID, _entry.hwid);


    if (updated) {
        SQLITE3_QUERY update_query = SQLITE3_QUERY(q); // ? will be replaced after bind
        if (this->db.execute(update_query) || this->db.commit()) { // execute query
            this->db.perror();
            return false;
        }
    }
    return true;
}

/*
bool db_adapter::update_game_db_entry(const std::string &_hwid = "", const std::string &_sid = "",
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





*/