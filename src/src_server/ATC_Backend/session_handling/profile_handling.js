var CONFIG = require('../config'); //include the cofnig file
var MDB = require('./mongo_db_connection'); //include mongo db connection
var LH = require('./lobby_handling'); //GET LOBBY HANDLING FUNCTIONS
var CR = require('../chess_related_logic/chess_ranking'); // GET CHESS RANKING FUNCTIONS
var UUID = require('uuid');




module.exports = {
    get_profile: function(_hwid, _callback){
        MDB.getProfileCollection().findOne({hwid:_hwid, DOCTYPE:"PROFILE"},function(err,res){
           _callback(err,res);
       });
   },

    get_profile_virtual_id: function(_vid, _callback){
        MDB.getProfileCollection().findOne({virtual_player_id:_vid, DOCTYPE:"PROFILE"},function(err,res){
            _callback(err,res);
        });
    },

    create_profile: function(_hwid, _playertype ,_callback){

        //GENERATE VIRTUAL ID
        var vid = String(UUID.v1());
        var fn = "INVALID_NAME";

        if(_playertype || _playertype === 1){
            _playertype = LH.PLAYER_TYPE.AI; //CPU PLAYER
            fn = "AI_" + vid.substr(0,5); //USE FIRST 5 CHARS OF VIRTUAL ID AS AN IDENTIFIER
        }else  if(!_playertype || _playertype === 0){
            _playertype = LH.PLAYER_TYPE.HUMAN; // HUMAN PLAYER / TABLE
            fn = "AWESOME_CHESS_PLAYER_" + vid.substr(0,5); //USE FIRST 5 CHARS OF VIRTUAL ID AS AN IDENTIFIER
        }

        var profile={hwid:_hwid, DOCTYPE:"PROFILE",account_created:Date.now(), rank:0,elo_rank_readable:CR.rank_to_elo_rating_name(0), friendly_name:fn,virtual_player_id:vid,player_type:_playertype};
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
    },

    apply_points_to_profile: function(_points,_game_id,_other_player,_callback){
        //TODO;
        //TODO GET PROFILE
        //UPDATE POINTS * time multiplier
        var point_elo_name = CR.rank_to_elo_rating_name(_points);
        console.log("#### USER POINTS ####")
        console.log(_points);
        console.log(point_elo_name);



        _callback(null,null);
    },
   simplify_profile_data: function (_profile_json) {
        //TODO COPY JSON SHOW ONLY IMPORTANT PROFILE INFORAMTION MASK THE OTHER
       //HIDE HWID
       //USE VIRTUAL ID
        return _profile_json;
   }
}