var CONFIG = require('../config'); //include the cofnig file
var MDB = require('./mongo_db_connection'); //include mongo db connection
var LH = require('./lobby_handling'); //GET LOBBY HANDLING FUNCTIONS
var CR = require('../chess_related_logic/chess_ranking'); // GET CHESS RANKING FUNCTIONS
var UUID = require('uuid');
var ANG = require('./AbstractNameGenerator'); //
function apply_points_to_profile(_points,_game_id,_other_player,_callback){
    //UPDATE POINTS * time multiplier
    var point_elo_name = CR.rank_to_elo_rating_name(_points);
    console.log("#### USER POINTS ####")
    console.log(_points);
    console.log(point_elo_name);
    //UPDATE PROFILE
    MDB.getProfileCollection().findOne({hwid:_other_player, DOCTYPE:"PROFILE"},function(gp_err,gp_res){
        console.log("------------- APPLY POINTS ", _points, " ", point_elo_name, " ", gp_err, gp_res, "--------------");
        if(gp_err || !gp_res){
            _callback(gp_err,null);
            return;
        }
        if(gp_res && gp_res.rank != null){
            gp_res.rank += _points;
        }else{
            _callback("gp_res.rank missing",null);
            return;
        }
        if(gp_res && gp_res.elo_rank_readable != null){
            gp_res.elo_rank_readable = point_elo_name;
        }else{
            _callback("gp_res.rank missing",null);
            return;
        }
        //FIANLLY UPDATE THE PROFILE EXTRY
        MDB.getProfileCollection().updateOne({hwid:_other_player},{$set:gp_res},function (uo_err,uo_res) {
            _callback(uo_err,"ok");
            return;
        });

    });

    //_callback(null,null);
}

function create_profile(_hwid, _playertype ,_callback){

    //GENERATE VIRTUAL ID
    var vid = String(UUID.v1());
    var fn = "INVALID_NAME";

    if(_playertype || _playertype === 1){
        _playertype = LH.PLAYER_TYPE.AI; //CPU PLAYER
        fn = "AI_" + vid.substr(0,5); //USE FIRST 5 CHARS OF VIRTUAL ID AS AN IDENTIFIER
    }else  if(!_playertype || _playertype === 0){
        _playertype = LH.PLAYER_TYPE.HUMAN; // HUMAN PLAYER / TABLE
        fn = ANG.generate_fullname_underscore()+ "_" + vid.substr(0,3); //USE FIRST 5 CHARS OF VIRTUAL ID AS AN IDENTIFIER
    }

    var profile={profile_config:{"SETTINGS":null,"USER_DATA":null},hwid:_hwid, DOCTYPE:"PROFILE",account_created:Date.now(), rank:0,elo_rank_readable:CR.rank_to_elo_rating_name(0), friendly_name:fn,virtual_player_id:vid,player_type:_playertype};
    MDB.getProfileCollection().insertOne(profile,function(err,res){
        if(err){
            _callback(err,res,profile);
            return;
        }
        //AT PROFILE CREATE INIT THE PLAYER IN THE GAME LOBBY TOO
        LH.set_player_lobby_state(_hwid,LH.player_state.idle,function (lh_err,lh_res) {
            _callback(lh_err,res,profile);
        });
    });
}

function get_profile(_hwid, _callback){
    MDB.getProfileCollection().findOne({hwid:_hwid, DOCTYPE:"PROFILE"},function(err,res){
        _callback(err,res);
    });
}

function get_profile_virtual_id(_vid, _callback){
    MDB.getProfileCollection().findOne({virtual_player_id:_vid, DOCTYPE:"PROFILE"},function(err,res){
        _callback(err,res);
    });
}





function get_player_config(_cfg, _hwid, _callback){
        //UPDATE PROFILE
        var profile_config = {cfg:_cfg};
        MDB.getProfileCollection().findOne({hwid:_hwid, DOCTYPE:"PROFILE"},function(gp_err,gp_res){
            _callback(gp_err,gp_res.profile_config);
        });
}

function set_player_config(_cfg,_hwid, _callback){
    //UPDATE PROFILE
    var profile_config = {cfg:_cfg};
    MDB.getProfileCollection().updateOne({hwid:_hwid},{$set:profile_config},function (uo_err,uo_res) {
        _callback(uo_err,"ok");
    });
}
module.exports = {





    simplify_profile_data: function (_profile_json) {
        return _profile_json;
   },
    get_profile,
    get_profile_virtual_id,
    create_profile,
    apply_points_to_profile,
    set_player_config,
    get_player_config
}