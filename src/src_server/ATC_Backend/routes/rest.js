/**
 * @module REST_API
 */
var express = require('express');
var router = express.Router();
var redisClient = require("../session_handling/redis_db_connection");
var UUID = require('uuid');
var profile_handling = require("../session_handling/profile_handling");
var lobby_handling = require("../session_handling/lobby_handling");
var session_handling = require("../session_handling/session_handler");
var game_handling = require("../session_handling/game_handler");
var CBL = require("../chess_related_logic/chess_board_logic");


/*
list all player profiles
list all games
list game with id
logout
client version
 */

/**
 # @INPUT_QUERY
 # @PROCESSING
 - simply returns a 200 with a status ok that the backend is running
 # @RETURN
 - err - true if an error occurs
 - status - the error if err set
 # @EXAMPLE
 - /rest/status -> {"err":false, "status":"ok"}
 */
router.get('/client_status',function (req,res,next) {
    var sysok = true;
    //TODO CHECK DB CONNECTIONS ...
    if(sysok){
        res.json({"err": !sysok, "status":"ok"});
        return;
    }else{
       // res.status(2);
        res.json({"err": !sysok, "status":"sysok error"})
    }
});

/**
# @INPUT_QUERY
- table_id (=hwid)
- session_key (=sid)
# @PROCESSING
 - if session_key matches the content in the redis db
 - the last seen timestamp will refreshed
# @RETURN
- err - if an error occurs
- status - ok if timestamp updated
- reply - database entry for debugging
 # @EXAMPLE
 - /rest/heartbeat?hwid=1334&sid=1234 -> {"err":null,"status":"ok","reply":null}
 */
router.get('/heartbeat', function(req, res, next) {
    //GET QUERY PARAMETERS SANITIZED
    var hwid = req.queryString("hwid");
    var sid = req.queryString("sid");
    //CHECK IF EVERY NEEDED IS SET
    if(!hwid || !sid){
       // res.status(500);
        res.json({err:true, status:"err_query_paramter_hwid_or_sid_not_set",reply:null});
        return;
    }
    redisClient.getRedisConnection().get("session:"+hwid, function(err, reply) {
        //CHECK FOR KEY EXISTS
        if(err || !reply){
         //   res.status(500);
            res.json({err:err,status:"err_db_read_error",reply:null});
            return;
        }
        //PARSE KEY JSON-DATA
        var data = null;
        try {
            data = JSON.parse(reply);
            if(!data){
                throw "err_json_parse_data_failed_or_data_are_null";
            }
        } catch (e) {
           // res.status(500);
            res.json({err:e,status:e,reply:null});
            return;
        }
        //CHECK SESSION_ID
        if(!data.session_id || data.session_id !== sid){
            //res.status(500);
            res.json({err:true,status:"err_session_id_wrong",reply:null});
            return;
        }
        //UPDATE TIMESTAMP
        data.timestamp = Date.now();
        redisClient.getRedisConnection().set("session:"+hwid, JSON.stringify(data),function (err_set,replay_set) {
            if(err || !reply){
              //  res.status(500);
                res.json({err:err,status:"err_db_update_failed",reply:null});
                return;
            }
            //RETURN INFO
            res.json({err:null,status:"ok_timestamp_update_success",reply:data.timestamp});
        });
    });
});

/**
 # @INPUT_QUERY
 - table_id (=hwid)
 # @PROCESSING
 - creates a user table entry in nosql database, generate sid
 - if user record exists nothing happend but redis will be updated and return sid
 - creates/updates a entry in redis database to accept /rest/heartbeat calls
 # @RETURN
 - err - if an error occurs
 - status - ok if login succeed
 - sid - the new session_id - use this for all other requests
 # @EXAMPLE
 - /rest/login?hwid=1334
 */
router.get('/login',function(req,res,next){
    var hwid = req.queryString("hwid");
    var playertype = req.queryInt("playertype");
    if(!hwid){
        //res.status(500);
        res.json({err:true, status:"err_hwid_or_playertype_not_set",sid:null,profile:null});
        return;
    }
    //CHECK REDIS FOR EXISTING SESSION
    redisClient.getRedisConnection().get("session:"+hwid, function(err, reply) {
        if(err){
            //res.status(500);
            res.json({err:err,status:"err_db_read_error",sid:null,profile:null});
            return;
        }

        if(!reply){//NO ACTIVE SESSION FOUND
            //CHECK IF THE PROFILE EXISTS
            profile_handling.get_profile(hwid,function (pe_err,pe_result) {
                if(pe_err){
                    //res.status(500);
                    res.json({err:pe_err,status:"err_db_get_profile_failed",sid:null});
                    return;
                }
                //GENERATE SESSION STUFF
                var session_id = UUID.v1();
                var ts = Date.now();

                //IF PROFILE RESULT IS NULL -> NO PROFILE EXITSTS ->CREATE PROFILE ADN START SESSION
                if(!pe_result){
                    profile_handling.create_profile(hwid,playertype,function (cp_err,cp_result,cp_profile) {
                        if(cp_err){
                            //res.status(500);
                            res.json({err:pe_err,status:"err_profile_creation_failed",sid:null});
                            return;
                        }
                        //CREATE SESSION ENTRY
                        redisClient.getRedisConnection().set("session:"+hwid, JSON.stringify({session_id:session_id,timestamp:ts, hwid:hwid}));
                        res.json({err:null, status:"ok",sid:session_id, timestamp:ts, profile:profile_handling.simplify_profile_data(cp_profile)});
                        return;
                    })
                }else{
                 //PROFILE ALREADY EXISTS -> BEGIN SESSION AND SEND PROFILE DATA
                    redisClient.getRedisConnection().set("session:"+hwid, JSON.stringify({session_id:session_id,timestamp:ts, hwid:hwid}));
                    //RESET LOBBY STATE -> no error chating if failed its ok because the db entry already exists
                    lobby_handling.set_player_lobby_state(hwid,lobby_handling.player_state.idle,function () {});
                    res.json({err:null, status:"ok",sid:session_id, timestamp:ts, profile:profile_handling.simplify_profile_data(pe_result)});
                    return;
                }
            });



        }else{ //IF A ENTRY ALREADY EXISTS -> USER LOGGED IN
            //res.status(500);
            res.json({err:err,status:"err_already_logged_in",sid:null, profile:null});
            return;
        }
    });
    //CHECK MONGO PROFILE DB
});




router.get('/logout',function(req,res,next){
    var hwid = req.queryString("hwid");
    var playertype = req.queryInt("playertype");
    if(!hwid){
        //res.status(500);
        res.json({err:true, status:"err_hwid_or_playertype_not_set",sid:null,profile:null});
        return;
    }
    //CHECK REDIS FOR EXISTING SESSION
    redisClient.getRedisConnection().get("session:"+hwid, function(err, reply) {
        if(err){
            //res.status(500);
            res.json({err:err,status:"err_db_read_error",sid:null,profile:null});
            return;
        }

        if(reply){//ACTIVE SESSION FOUND
            //TODO DELETE REDIS KEY
            //SET PLAYER STATE TO OFFLINE / NOT LISTED = 0
            //CANCEL MATCHES
            redisClient.getRedisConnection().del("session:"+hwid,function (err, reply) {
                if(err || !reply){
                    console.error("db_del_failed");
                }
                //SET PLAYER TO OFFLINE
                lobby_handling.remove_player_from_lobby(hwid,function (spl_err,spl_res) {
                    //CANCEL MATCH IF A MATCH IS RUNNING
                    game_handling.cancel_match_for_player(hwid,function (cmp_err,cmp_res){
              //          res.status(200);
                        res.json({err:err,cmp_err:cmp_err,spl_err:spl_err,status:"ok",sid:null, profile:null});
                        return;
                    });
                },false);
            });
        }else{ //IF A ENTRY ALREADY EXISTS -> USER LOGGED IN
            //res.status(500);
            res.json({err:err,status:"err_already_logged_out",sid:null, profile:null});
            return;
        }
    });
    //CHECK MONGO PROFILE DB
});





/**
 # @INPUT_QUERY
 - table_id (=hwid)
 - session_id (=sid)
 - pstate (=ps)
 # @PROCESSING
 - set the player state for the given player in the database according the ps query value
 - this call is used to set the plaer state after login for example to enable matchmaking or starting a new game
 # @RETURN
 - err - if an error occurs
 - status - ok switch ok
 # @EXAMPLE
 - /rest/set_player_state?hwid=123&sid=8935c2d0-cad8-11ea-b7a0-ebf94b1bb0f7&ps=1
 */
router.get('/set_player_state',function (req,res,next) {
    var hwid = req.queryString("hwid");
    var sid = req.queryString("sid");
    var pstate = req.queryInt("ps");
    //CHECK INPUT PARAMETERs
    if(!hwid || !sid || !pstate){
        //res.status(500);
        res.json({err:true, status:"err_query_paramter_hwid_or_sid_or_ps_not_set"});
        return;
    }
    //CHECK PS STATE
    if(!lobby_handling.check_int_for_valid_player_state(pstate)){
        //res.status(500);
        res.json({err:true, status:"err_invalid_player_state_ps_value"});
        return;
    }
    //CHECK_SESSION
    session_handling.check_valid_session(hwid,sid,function (cvs_err,cvs_res) {
        if(cvs_err){
            //res.status(500);
            res.json({err:cvs_err, status:"err_session_check_failed"});
            return;
        }
        //IF SESSION VALID -> CHANGE PLAYERSTATE
        if(cvs_res){
         //   lobby_handling.set
            lobby_handling.set_player_lobby_state(hwid,pstate,function (spl_err,spl_res) {
                res.json({err:spl_err, status:"ok"});
                return;
            });
        }else{
            //res.status(500);
            res.json({err:cvs_err, status:"err_session_key_sid_check_failed"});
            return;
        }
    })
});



// TODO ->START GAME WITH Player PSEUDO ID!!


/**
 # @INPUT_QUERY
 - table_id (=hwid)
 - session_id (=sid)
 # @PROCESSING
 - returns a list of all human players, they are in seachring for matchmaking state
 - so the player can start a game with one of these players
 # @RETURN
 - err - if an error occurs
 - status - error reason reaable
 - players - returns reach playerprofile that is in seachring for a match state
 # @EXAMPLE
 - /rest/get_players_avariable -> {"err":null,"status":"ok","players":[{profile},{profile}]}
 */
router.get('/get_players_avariable',function (req,res,next) {
    var hwid = req.queryString("hwid");
    var sid = req.queryString("sid");

    //CHECK INPUT PARAMETERs
    if(!hwid || !sid ){
        //res.status(500);
        res.json({err:true, status:"err_query_paramter_hwid_or_sid_or_not_set", players:null,count:0});
        return;
    }

    //CHECK_SESSION
    session_handling.check_valid_session(hwid,sid,function (cvs_err,cvs_res) {
        if(cvs_err){
            //res.status(500);
            res.json({err:cvs_err, status:"err_session_check_failed", players:null,count:0});
            return;
        }
        //IF SESSION VALID -> LIST PLAYERS
        if(cvs_res){
            //   lobby_handling.set
            lobby_handling.get_avariable_players(hwid,function (spl_err,spl_res) {
                res.json({err:spl_err, status:"ok", players:spl_res, count:spl_res.length});
                return;
            });
        }else{
            //res.status(500);
            res.json({err:cvs_err, status:"err_session_key_sid_check_failed",players:null,count:0});
            return;
        }
    })
});

// TODO REMOVE
router.get('/gmm',function (req,res,next) {
   CBL.get_board(CBL.get_start_opening_fen(),CBL.get_start_opening_fen(),CBL.PLAYER_TURN.BLACK,false,function (_err,_res) {
        res.json({err:_err,res:_res});
   })
});

/**
 # @INPUT_QUERY
 # @PROCESSING
 - get the number of free online ai players
 - the system should have a number of free players if no other human player is avariable for a match
 # @RETURN
 - err - if an error occurs
 - count - the number of free/able to play AI players
 - detailed - returns for each ai player the player profile
 # @EXAMPLE
 - /rest/get_avariable_ai_players -> {"err":null,"count":3,"detailed":[{profile},{profile}]}
 */
router.get('/get_avariable_ai_players',function (req,res,next) {
    lobby_handling.get_avariable_ai_player(function (e,r) {
        res.json({err:e,count:r.length, detailed:r});
    });
});






/**
 # @INPUT_QUERY
 - table_id (=hwid)
 - session_key (=sid)
 # @PROCESSING
 - returns all needed information for a players game state
 - also the player state eg which screen to show;
 # @RETURN
 - err - if an error occurs
 - status - ok if timestamp updated
 - matchmaking_state - the current matchmaking state for the player; eg is waiting or searching; is null is a game is running
 - game_state - if a game is running it contains all nedded information for the player;is_game_running,is_my_turn, the current fen of the board,...
 # @EXAMPLE
 -TODO /rest/get_player_state?hwid=1334&sid=1234 -> {"err":null,"status":"ok","matchmaking_state":null,"game_state":null}
 */
router.get('/get_player_state',function (req,res,next) {
    var hwid = req.queryString("hwid");
    var sid = req.queryString("sid");
    var simplified = req.queryInt("simplified");


    //CHECK INPUT PARAMETER
    if(!hwid || !sid ){
        //res.status(500);
        res.json({err:true, status:"err_query_paramter_hwid_or_sid_or_not_set", matchmaking_state:null,game_state:null});
        return;
    }

    //CHECK_SESSION
    session_handling.check_valid_session(hwid,sid,function (cvs_err,cvs_res) {
        if(cvs_err){
            //res.status(500);
            res.json({err:cvs_err, status:"err_session_check_failed",  matchmaking_state:null,game_state:null});
            return;
        }
        //IF SESSION VALID -> LIST PLAYERS
        if(cvs_res){
            //   GET THE MATCHMAKING STATE
            lobby_handling.get_player_matchmaking_state(hwid,function (spl_err,spl_res) {
                //SIMPLE TRUE FALSE RETURN
                var mm_wait_state = false;
                if(spl_res){
                    mm_wait_state = true;
                }
                //IF A GAME IS RUNNING GET
                game_handling.get_player_active_game_state(hwid,function (gs_err,gs_res, gs_simplified) {
                    //CHECK IF GAME IN GENERAL RUNNING
                    var mm_game_state = false;
                    if(gs_res){
                        mm_game_state = true;
                    }
                    if(!simplified) {
                        res.json({
                            err: null,
                            status: "ok",
                            matchmaking_state: {detailed: spl_res, err: spl_err, waiting_for_game: mm_wait_state},
                            game_state: {detailed: gs_res, simplified:gs_simplified ,err: gs_err, game_running: mm_game_state}
                        });

                    }else{
                        res.json({
                            err: null,
                            status: "ok",
                            matchmaking_state: {detailed: spl_res, err: spl_err, waiting_for_game: mm_wait_state},
                            game_state: {detailed: null,simplified:gs_simplified, err: gs_err, game_running: mm_game_state}
                        });

                    }

                });
            });
        }else{
            //res.status(500);
            res.json({err:cvs_err, status:"err_session_key_sid_check_failed", matchmaking_state:null,game_state:null});
        }
    });
});

/**
 # @INPUT_QUERY
 - table_id (=hwid)
 - session_key (=sid)
 # @PROCESSING
 - if a game is in init state; the player must confirm that his game board matching the fen in the game_state
 - after both player has confirmed; that their setup is finished the game will start
 # @RETURN
 - err - if an error occurs
 - status - ok with success
  # @EXAMPLE
 - /rest/player_setup_confirmation?hwid=1334&sid=1234 -> {"err":null,"status":"ok"}
 */
router.get('/player_setup_confirmation',function (req,res,next) {
    var hwid = req.queryString("hwid");
    var sid = req.queryString("sid");

    //CHECK INPUT PARAMETER
    if(!hwid || !sid ){
        //res.status(500);
        res.json({err:true, status:"err_query_paramter_hwid_or_sid_or_not_set", matchmaking_state:null,game_state:null});
        return;
    }

    //CHECK_SESSION
    session_handling.check_valid_session(hwid,sid,function (cvs_err,cvs_res) {
        if(cvs_err){
          //  res.status(500);
            res.json({err:cvs_err, status:"err_session_check_failed"});
            return;
        }
        //IF SESSION VALID -> UPDATE PLAYER STATE
        if(cvs_res){
          game_handling.set_player_setup_confirmation(hwid,function (psc_err,psc_res) {
                res.json({err:psc_err, status:psc_res});
            });
        }else{
            //res.status(500);
            res.json({err:cvs_err, status:"err_session_key_sid_check_failed"});
        }
    });
});

/**
 # @INPUT_QUERY
 - table_id (=hwid)
 - session_key (=sid)
 - move (=move in format A1B2, C3C4,.. format, upper or lowercase)
 # @PROCESSING
 - first check if a game is running for the current player
 - checks also if its players_turn
 - the move will be checked with the move validator service; if its a valid move for the given board
 - if its correct the move will be executed on the current game board
 - the new board can be retrieved with the get_player_state call
 # @RETURN
 - err - if an error occurs
 - status - ok if success move was successfully made
 # @EXAMPLE
 - /rest/make_move?hwid=1334&sid=1234&move=e3e4 -> {"err":null,"status":"ok"}
 */
router.get('/make_move',function (req,res,next) {
    var hwid = req.queryString("hwid");
    var sid = req.queryString("sid");
    var move = req.queryString("move");



    //CHECK INPUT PARAMETER
    if(!hwid || !sid  || !move){
        //res.status(500);
        res.json({err:true, status:"err_query_paramter_hwid_or_sid_or_move_not_set"});
        return;
    }

    //CHECK_SESSION
    session_handling.check_valid_session(hwid,sid,function (cvs_err,cvs_res) {
        if(cvs_err){
          //  res.status(500);
            res.json({err:cvs_err, status:"err_session_check_failed"});
            return;
        }
        //IF SESSION VALID -> UPDATE PLAYER STATE
        if(cvs_res){
            game_handling.make_move(hwid,move,function (psc_err,psc_res) {
                res.json({err:psc_err, status:psc_res});
            });
        }else{
           // res.status(500);
            res.json({err:cvs_err, status:"err_session_key_sid_check_failed"});
        }
    });
});

module.exports = router;
