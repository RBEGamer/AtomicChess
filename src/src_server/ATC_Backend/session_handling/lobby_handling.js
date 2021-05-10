var mongo = require('mongodb');
var MongoClient = require('mongodb').MongoClient;
var CFG = require('../config/config'); //include the cofnig file
var mdb = require('./mongo_db_connection'); //include mongo db connection
var cr = require("../chess_related_logic/chess_ranking");
//THE PLAYER GAME STATES
// idle -> wird nicht gelistet
// hosting_game -> wartet auf spieler
// searching_matchmake -> freier spieler; wird per matchmaking zugewiesen
// searching_manual -> freier psiler; kann nicht zugewiesen werden (= sieht hosting spieler)
const player_state = Object.freeze({
    INVALID:0,
    idle: 1,
    hosting_game_manual: 2,
    searching_matchmake: 4,
    searching_manual:5,
    preparing_ingame:6,
    ingame:7

});
const PLAYER_TYPE = Object.freeze({
    HUMAN:0,
    AI:1,
    HUMAN_VIRTUAL:2

});
function get_avariable_players(_own_hwid,_callback, _ps = player_state.searching_matchmake, _include_hwid = false){
    //GET ALL PLAYERS IN THE LOBBY JOIN THE PROFILE TABLE INTO TO GET THEIR PROFILE NAMES AND RANK TOO
    mdb.getLobbyCollection().aggregate([{ $lookup:
            {
                from: CFG.getConfig().mongodb_collection_profiles,
                localField: 'hwid',
                foreignField: 'hwid',
                as: 'profile'
            }
    },{
        $match:{
            player_state:_ps,
            DOCTYPE: "LOBBY",
            has_valid_sesstion: true //FILTER ONLY ACTIVE USERS => FALG SET BY LOGIN AND UNSET BY INATIVITY
        }

    }]).toArray(function(err,res){
        if(err || !res){
            _callback(err,null);
            return;
        }
        //MAKE A NICE ARRAY OF PLAYERS WITH THEIR VIRTUAL_PLAYER_ID AND THE NAME WITH RANK
        var player_list = [];
        for(var i= 0;i <res.length;i++ ){

            //IGNORE OWN PLAYER IF SET -> ELSE LIST ALL PLAYERS
            if(_own_hwid && _own_hwid === res[i].hwid){
                continue;
            }

            var name = "SECRET_PLAYER";
            var rank = -1;
            var vid = null;
            var ptype = 0;
            var hwid = null;

            if(_include_hwid){
                hwid = res[i].hwid;
            }

            if(res[i].profile && res[i].profile.length > 0 && res[i].profile[0].virtual_player_id){
                name = res[i].profile[0].friendly_name;
                rank = res[i].profile[0].rank;
                vid = res[i].profile[0].virtual_player_id;
                ptype = res[i].profile[0].player_type;
            }else{
                continue;
            }
            //RETURN ONLY THE VIRTUAL PLAYER ID SO THE HWID IS PROTECTED AND WILL ONLY USED BY THE TABLE
            player_list.push({visible_name:name, rank:rank,virtual_player_id:vid, elo_rank:cr.rank_to_elo_rating_name(rank), player_type:ptype,state_switched_time:res[i].state_switched_time,hwid:hwid});
        }
        _callback(null,player_list);
        return;
    });
}

function get_player_in_lobby(_callback){
    //GET ALL PLAYERS IN THE LOBBY JOIN THE PROFILE TABLE INTO TO GET THEIR PROFILE NAMES AND RANK TOO
    mdb.getLobbyCollection().aggregate([{ $lookup:
            {
                from: CFG.getConfig().mongodb_collection_profiles,
                localField: 'hwid',
                foreignField: 'hwid',
                as: 'profile'
            }
    },{
        $match:{
            DOCTYPE: "LOBBY",
        }

    }]).toArray(function(err,res){
        if(err || !res){
            _callback(err,null);
            return;
        }
        _callback(null,res);
        return;
    });
}

function get_player_for_matchmaking(_callback, _include_hwid =false){
    //GET ALL PLAYERS AVARIABLE IN THE LOBBY
    //SORT THEM BETWEEN HUMAN AND AI PLAYER
    get_avariable_players(null,function (err_sm,res_sm) {
            //CHECK ERR
            if(err_sm){
                _callback(err_sm,{combined_player_searching:[], player_searching_human: [],player_searching_ai:[]});
                return;
            }
            //res_sm -> ALL PLAYERS
            //SORT PLAYER TYPES AI HUMAN
            var tmp_player_ai = [];
            var tmp_player_human = [];
            for(var i=0;i < res_sm.length;i++){
                if(res_sm[i].player_type === PLAYER_TYPE.AI){ // TYPE 1 => AI
                    tmp_player_ai.push(res_sm[i]);
                }else  if(res_sm[i].player_type === PLAYER_TYPE.HUMAN || res_sm[i].player_type === PLAYER_TYPE.HUMAN_VIRTUAL) { // TYPE 0 => HUMAN
                    tmp_player_human.push(res_sm[i]);
                }
            }
            //RETURN PLAYERS
            _callback(null,{combined_player_searching:res_sm, player_searching_human:tmp_player_human,player_searching_ai:tmp_player_ai});
    },player_state.searching_matchmake,_include_hwid)
}

function get_avariable_ai_player(_callback){
    get_player_for_matchmaking(function (err_sm,res_sm) {
        if(err_sm || !res_sm || !res_sm.player_searching_ai){
            _callback(err_sm,[]);
            return;
        }
        _callback(null,res_sm.player_searching_ai);
        return;
    });
}

function get_player_matchmaking_state(_hwid,_callback){
    mdb.getLobbyCollection().findOne({hwid:_hwid,     player_state:player_state.searching_matchmake, DOCTYPE: "LOBBY"},function (err,res) {
    if(err || !res){
        _callback(err,null);
        return
    }
    _callback(null,res);
    return;
    });
}

function remove_lobby_entry(_hwid,_callback){
    if(_hwid === undefined || _hwid == null){
        _callack("_hwid is undefined", null);
        return;
    }
    mdb.getLobbyCollection().deleteOne({DOCTYPE: "LOBBY",hwid: _hwid},function (err,res) {
        _callback(err,res);
    });
}

module.exports = {
    get_player_for_matchmaking,
    get_avariable_players,
    get_avariable_ai_player,
    get_player_in_lobby,
    remove_lobby_entry,
    set_player_lobby_state: function(_hwid,_state, _callback){
        //ADD OR UPDATE THE LOBBY ENTRY FOR EACH PLAYER
        //THE UPSERT OPTION MAKES POSSIBLE TO ADD THE ENTRY IF NOT EXISTS
        mdb.getLobbyCollection().update({DOCTYPE: "LOBBY",hwid: _hwid}, { $set: { hwid: _hwid,last_seen:Date.now(),player_state: _state,state_switched_time:Date.now() }}, { upsert: true },function (err,res) {
        _callback(err,res);
        });
   },

    set_valid_session_flag: function (_hwid, _session_flag_state, _callback){
        var query = { hwid: _hwid,has_valid_sesstion: _session_flag_state };
        //UPDATE LAST_SEEN ONLY IF PLAYER ACTIVELY SET HIS SESSION TO ACTIVE/TRUE
        //FALG CAN BE SET TO FALSE BY THE BACKEND AUTOMATICALLY
        if(_session_flag_state){
            query.last_seen = Date.now();
        }
        mdb.getLobbyCollection().update({DOCTYPE: "LOBBY",hwid: _hwid}, { $set: query}, { upsert: true },function (err,res) {
            _callback(err,res);
        });
    },
    get_valid_session_flag: function (_hwid,_callback){
        MDB.getLobbyCollection().findOne({hwid:_hwid, DOCTYPE: "LOBBY"},function (err,res) {
            if(!err && res){
                 _callback(null,res.has_valid_sesstion);
                 return;
            }
            _callback(err,false);
            return
        });
    },
    check_int_for_valid_player_state: function(_ps){
        if(_ps === player_state.ingame ||_ps === player_state.idle || _ps === player_state.hosting_game_manual || _ps === player_state.hosting_game_matchmake || _ps === player_state.searching_matchmake || _ps === player_state.searching_manual){
            return true;
        }
        return false;
    },

    remove_player_from_lobby: function(_hwid, _callback, _set_only_offline){
            this.set_player_lobby_state(_hwid,player_state.idle,_callback);
    },

    //EXPOSED VARAIBLES
    player_state,
    get_player_matchmaking_state,
    PLAYER_TYPE
}

