var mongo = require('mongodb');
var MongoClient = require('mongodb').MongoClient;
var CONFIG = require('../config'); //include the cofnig file
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

function get_avariable_players(_own_hwid,_callback, _ps = player_state.searching_matchmake, _include_hwid = false){
    //GET ALL PLAYERS IN THE LOBBY JOIN THE PROFILE TABLE INTO TO GET THEIR PROFILE NAMES AND RANK TOO
    mdb.getLobbyCollection().aggregate([{ $lookup:
            {
                from: CONFIG.getConfig().mongodb_collection_profiles,
                localField: 'hwid',
                foreignField: 'hwid',
                as: 'profile'
            }
    },{
        $match:{
            player_state:_ps, DOCTYPE: "LOBBY"
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

//TODO SIMPLYFY FOR ONLY SEARCHING_MATCHMAKE
function get_player_for_matchmaking(_callback, _include_hwid =false){
    //alle hosts und searching for matchmaking
    get_avariable_players(null,function (err_sm,res_sm) {
        //GET ALL PLAYER SEARCHING_MATCHMAKE
        get_avariable_players(null,function (err_hm, res_hm) {
            //CHECK ERR
            if(err_sm || err_hm){
                _callback({err_hm:err_hm,err_sm:err_sm},{combined_player_searching:[], player_searching_mm: res_sm,player_hosting_mm:res_hm});
                return;
            }
     //       console.log("-- get_player_for_matchmaking --")
    //        console.log(res_hm);
            //CHECK ERR
            //->COMPBINE
            //DEBUG RESULT
            var tmp = res_sm.concat(res_hm);
            _callback(null,{combined_player_searching:tmp, player_searching_mm: res_sm,player_hosting_mm:res_hm});
        },player_state.searching_matchmake,_include_hwid);
    },player_state.searching_matchmake,_include_hwid)
}



function get_avariable_ai_player(_callback){
    get_player_for_matchmaking(function (err_sm,res_sm) {
        if(err_sm){
            _callback(err_sm,[]);
            return;
        }
        var tmp = [];
        for(var i=0;i < res_sm.combined_player_searching.length;i++){
            if(res_sm.combined_player_searching[i] && res_sm.combined_player_searching[i].player_type){
                tmp.push(res_sm.combined_player_searching[i]);
            }
        }
        _callback(null,tmp);
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

module.exports = {
    get_player_for_matchmaking,
    get_avariable_players,
    get_avariable_ai_player,
    set_player_lobby_state: function(_hwid,_state, _callback){
        //ADD OR UPDATE THE LOBBY ENTRY FOR EACH PLAYER
        //THE UPSERT OPTION MAKES POSSIBLE TO ADD THE ENTRY IF NOT EXISTS
        mdb.getLobbyCollection().update({DOCTYPE: "LOBBY",hwid: _hwid}, { $set: { hwid: _hwid,last_seen:Date.now(),player_state: _state,state_switched_time:Date.now() }}, { upsert: true },function (err,res) {
        _callback(err,res);
        });
   },



    check_int_for_valid_player_state: function(_ps){
        if(_ps === player_state.ingame ||_ps === player_state.idle || _ps === player_state.hosting_game_manual || _ps === player_state.hosting_game_matchmake || _ps === player_state.searching_matchmake || _ps === player_state.searching_manual){
            return true;
        }
        return false;
    },

    remove_player_from_lobby: function(_hwid, _callback, _set_only_offline){
        if(_set_only_offline){
            this.set_player_lobby_state(_hwid,player_state.offline,_callback);
        }else{
            mdb.getLobbyCollection().deleteOne({DOCTYPE: "LOBBY",hwid:_hwid},_callback);
        }
    },
    //EXPOSED VARAIBLES
    player_state,
    get_player_matchmaking_state
}

