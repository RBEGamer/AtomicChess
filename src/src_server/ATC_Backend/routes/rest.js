/**
 * @module REST_API
 */
var express = require('express');
var router = express.Router();
var sanitizer = require('sanitizer');
var bcrypt = require('bcrypt'); //for pw hash
//---- LOGIC PACKAGES -------------- //
var redisClient = require("../session_handling/redis_db_connection");
var profile_handling = require("../session_handling/profile_handling");
var lobby_handling = require("../session_handling/lobby_handling");
var session_handling = require("../session_handling/session_handler");
var game_handling = require("../session_handling/game_handler");
var CBL = require("../chess_related_logic/chess_board_logic");
var CFG = require("../config/config"); //include the cofnig file
var MDB = require("../session_handling/mongo_db_connection"); //ONLY USED IN client_status
var RDS = require("../session_handling/redis_db_connection"); //ONLY USED IN client_status




/**
 # @INPUT_QUERY
 # @PROCESSING
 - simply returns a 200 with a status ok that the backend is running
 # @RETURN
 - err - true if an error occurs
 - status - the error if err set
 # @EXAMPLE
 - /rest/status -> {"err":null, "status":"ok"}
 - /rest/status -> {"err":true, "status":"error"}
 */
router.get('/client_status',function (req,res,next) {
    try {
    if(MDB.state() && RDS.state()){
        res.json({"err": null, "status":"ok"});
        return;
    }else{
        res.json({"err": sysok, "status":null})
    }
    }catch (e) {
        res.json({"err": e, "status":null})
    }
});

/**
 # @INPUT_QUERY
 # @PROCESSING
 - simply returns a 200 with a move_validator_state is null that the move_validator microservice is running
 # @RETURN
 - move_validator_state - is not null if an error occured/movement validator is offline
 # @EXAMPLE
 - /rest/service_state -> {"move_validator_state":null}
 - /rest/service_state -> {"move_validator_state":"service not reachable"}
 */
router.get('/service_state',function (req,res,next) {
    try{
    CBL.check_move_validator_state(function (cmvs_err){
        res.json({err:null,"move_validator_state":!cmvs_err,"move_validator_state_err":cmvs_err});
    });
    }catch (e) {
        res.json({"err": e, "move_validator_state":null});
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
    try{
    //GET QUERY PARAMETERS SANITIZED
    var hwid = req.queryString("hwid");
    var sid = req.queryString("sid");
    //CHECK IF EVERY NEEDED IS SET
    if(!hwid || !sid){
       // res.status(500);
        res.json({err:true, status:"err_query_paramter_hwid_or_sid_not_set",reply:null});
        return;
    }
    session_handling.update_session_timestamp(hwid,sid,function (ust_err,ust_res){
        res.json({err:ust_err,status:ust_res});
        return;
    });

    }catch (e) {
        res.json({err:e,status:null});
        return;
    }
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
    try{
    var hwid = req.queryString("hwid");
    var playertype = req.queryInt("playertype");

    if(!hwid){
        res.json({err:true, status:"err_hwid_or_playertype_not_set",sid:null,profile:null});
        return;
    }

    //CHECK REDIS FOR EXISTING SESSION
    redisClient.getRedisConnection().get("session:"+hwid, function(err, reply) {
        if(err){
            res.json({err:err,status:"err_db_read_error",sid:null,profile:null});
            return;
        }

        if(!reply){//NO ACTIVE SESSION FOUND
            //CHECK IF THE PROFILE EXISTS
            profile_handling.get_profile(hwid,function (pe_err,pe_result) {
                if(pe_err){
                    res.json({err:pe_err,status:"err_db_get_profile_failed",sid:null});
                    return;
                }
                //IF PROFILE RESULT IS NULL -> NO PROFILE EXITSTS ->CREATE PROFILE ADN START SESSION
                if(!pe_result){
                    profile_handling.create_profile(hwid,playertype,function (cp_err,cp_result,cp_profile) {
                        if(cp_err){
                            res.json({err:pe_err,status:"err_profile_creation_failed",sid:null});
                            return;
                        }
                        //CREATE SESSION ENTRY
                        session_handling.insert_session(hwid, function (is_err,is_res){
                            if(is_err){
                                res.json({err:pe_err,status:"err_insert_session_failed",sid:null});
                                return;
                            }
                            //SET VALID SESSION FLAG
                            lobby_handling.set_valid_session_flag(hwid,true,function (svfs_err,svfs_res){
                                res.json({err:null, status:"ok",sid:is_res, profile:profile_handling.simplify_profile_data(cp_profile)});
                                return;
                            });
                        });
                    });
                }else{
                    //PROFILE ALREADY EXISTS -> BEGIN SESSION AND SEND PROFILE DATA
                    session_handling.insert_session(hwid,function (is_err,is_res){
                        if(is_err){
                            res.json({err:pe_err,status:"err_update_session_failed",sid:null});
                            return;
                        }
                        //RESET LOBBY STATE -> no error chating if failed its ok because the db entry already exists
                        lobby_handling.set_player_lobby_state(hwid,lobby_handling.player_state.idle,function (spls_err,spls_res) {
                            lobby_handling.set_valid_session_flag(hwid,true,function (svfs_err,svfs_res){
                                res.json({err:null, status:"ok",sid:is_res, profile:profile_handling.simplify_profile_data(pe_result)});
                                return;
                            });
                        });
                    });
                }
            });
            //IF A ENTRY ALREADY EXISTS -> USER LOGGED IN
        }else{
            //res.status(500);
            res.json({err:err,status:"err_already_logged_in",sid:null, profile:null});
        }
    });
    }catch (e) {
        res.json({err:e,status:null,sid:null, profile:null});
        return;
    }
});

/**
 # @INPUT_QUERY
 - table_id (=hwid)
 # @PROCESSING
 - logouts the user, which is used to create a new session_id with login
 - cancel active games / sessions
 # @RETURN
 - err - if an error occurs
 - status - ok if logout succeed
 # @EXAMPLE
 - /rest/logout?hwid=1334
 */
router.get('/logout',function(req,res,next){
    try{
    var hwid = req.queryString("hwid");

    if(!hwid){
        res.json({err:true, status:"err_hwid_or_playertype_not_set"});
        return;
    }
    //CHECK REDIS FOR EXISTING SESSION
    redisClient.getRedisConnection().get("session:"+hwid, function(err, reply) {
        if(err){
            res.json({err:err,status:"err_db_read_error"});
            return;
        }
        //ACTIVE SESSION FOUND
        if(reply){
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
                        res.json({err:err,cmp_err:cmp_err,spl_err:spl_err,status:"ok"});
                        return;
                    });
                },false);
            });
        }else{ //IF A ENTRY ALREADY EXISTS -> USER LOGGED IN
            res.json({err:err,status:"err_already_logged_out"});
            return;
        }
    });
    }catch (e) {
        res.json({err:e,status:null});
        return;
    }
});

router.get('/abort_game',function (req,res,next) {
    try{
        var hwid = req.queryString("hwid");
        var sid = req.queryString("sid");

        //CHECK INPUT PARAMETERs
        if(!hwid || !sid){
            //res.status(500);
            res.json({err:true, status:"err_query_paramter_hwid_or_sid_or_ps_not_set"});
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
                lobby_handling.remove_player_from_lobby(hwid,function (spl_err,spl_res) {
                    //CANCEL MATCH IF A MATCH IS RUNNING
                    game_handling.cancel_match_for_player(hwid,function (cmp_err,cmp_res){
                        res.json({err:err,cmp_err:cmp_err,spl_err:spl_err,status:"ok"});
                        return;
                    });
                },false);
            }else{
                //res.status(500);
                res.json({err:cvs_err, status:"err_session_key_sid_check_failed"});
                return;
            }
        });
    }catch (e) {
        res.json({err:e, status:null});
        return;
    }
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
    try{
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
    });
    }catch (e) {
        res.json({err:e, status:null});
        return;
    }
});

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
    try{
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
    });
    }catch (e) {
        res.json({err:e, status:null,players:null,count:0});
        return;
    }
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
    try{
    lobby_handling.get_avariable_ai_player(function (e,r) {
        res.json({err:e,count:r.length, detailed:r});
    });
    }catch (e) {
        res.json({err:e,count:0, detailed:null});
        return;
    }
});

/**
 # @INPUT_QUERY
 # @PROCESSING
 - returns all game records starting/running/finished state
 # @RETURN
 - err - if an error occurs
 - count - count of records
 - game_data - an array, which contains simplied information (id,gamestate, creation_date) of each game
 # @EXAMPLE
 - /rest/get_game_list -> {"err":null,"count":0,"game_data":[{game_id:"xyz","game_state":"xyz,.."}]}
 */
router.get('/get_game_list',function (req,res,next) {
    try{
    game_handling.list_all_games(function (lag_err,lag_res){
        if(lag_err || !lag_res){
            res.json({err:lag_err,count:game_data.length, game_data:game_data});
        }
        var game_data= [];
        for (let i = 0; i < lag_res.length; i++) {
            var name_pw = "---";
            var name_pb = "---";
            var pid_pw = "";
            var pid_pb = "";
            //CHECK IF PROFILE WAS FOUND FOR FIRENDLY NAME
            if(lag_res[i].profile_white && lag_res[i].profile_white[0] && lag_res[i].profile_white[0].friendly_name){
                name_pw = lag_res[i].profile_white[0].friendly_name;
            }
            if(lag_res[i].profile_black && lag_res[i].profile_black[0] && lag_res[i].profile_black[0].friendly_name){
                name_pb = lag_res[i].profile_black[0].friendly_name;
            }


            //GET PROFILE VIRTUAL ID
            if(lag_res[i].profile_white && lag_res[i].profile_white[0] && lag_res[i].profile_white[0].virtual_player_id){
                pid_pw = lag_res[i].profile_white[0].virtual_player_id;
            }
            if(lag_res[i].profile_black && lag_res[i].profile_black[0] && lag_res[i].profile_black[0].virtual_player_id){
                pid_pb = lag_res[i].profile_black[0].virtual_player_id;
            }

            //POPULATE RESULT ARRAY
            game_data.push({
                game_id:lag_res[i].id,
                created_timestamp: lag_res[i].last_game_interaction,
                game_state:lag_res[i].game_state,
                player_white_profile_name:name_pw,
                player_black_profile_name:name_pb,
                player_white_profile_id:pid_pw,
                player_black_profile_id:pid_pb,
                move_count:lag_res[i].turn_history.length
            });
        }
        res.json({err:null,count:game_data.length, game_data:game_data});
    });
    }catch (e) {
        res.json({err:e,count:0, game_data:null});
        return;
    }
});

/**
 # @INPUT_QUERY
 - gid (=game record id)
 # @PROCESSING
 - return a game records withmore information
 # @RETURN
 - err - if an error occurs
 - game_data - an array, which contains simplied information (id,gamestate, creation_date, turns history) of the selected game
 # @EXAMPLE
 - /rest/get_game -> {"err":null,"game_data":[{id:"xyz","game_state":"xyz,..", turn_history:[]}]}
 */
router.get('/get_game',function (req,res,next) {
    try{
    var gid = req.queryString("gid");
    if(!gid || gid === ""){
        res.json({err:"gid not set",game_data:null});
        return;
    }

    game_handling.get_game(gid,function (lag_err,lag_res){
            if(lag_err || !lag_res){
                res.json({err:lag_err, game_data:null});
            }
            //SERVE ONLY NEEDED INFORMATION ABOUT THE GAME
           var game_information =  {
                game_id:lag_res.id,
                created_timestamp: lag_res.last_game_interaction,
                game_state:lag_res.game_state,
                start_board_string:lag_res.start_board_string,
               current_board:lag_res.current_board,
                move_count:lag_res.turn_history.length,
               turn_history:lag_res.turn_history
            };
            res.json({err:null, game_data:game_information});
    });
    }catch (e) {
        res.json({err:e, game_data:null});
        return;
    }
});

/**
 # @INPUT_QUERY
 - pid (=virtual_profile_id)
 # @PROCESSING
 - returns a profile record
 # @RETURN
 - err - if an error occurs
 - profile_data - an object, which contains simplied information (firendly_name, creation_date, rank, elo rank) of the selected profile
 # @EXAMPLE
 - /rest/get_profile_information -> {"err":null,"profile_data":[{rank:"xyz","elo_rank":"xyz,.."}]}
 */
router.get('/get_profile_information',function (req,res,next) {
    try{
    var pid = req.queryString("pid");
    if(!pid || pid === ""){
        res.json({err:"pid not set",profile_data:null});
        return;
    }
    //FETCH PROFILE DATA
    profile_handling.get_profile_virtual_id(pid,function (lag_err,lag_res){
        if(lag_err || !lag_res){
            res.json({err:lag_err, profile_data:null});
        }
        //SERVE ONLY NEEDED INFORMATION ABOUT THE GAME
        var profile_information =  {
            player_type:lag_res.player_type,
            virtual_player_id: lag_res.virtual_player_id,
            friendly_name:lag_res.friendly_name,
            elo_rank_readable:lag_res.elo_rank_readable,
            rank:lag_res.rank,
            account_created:lag_res.account_created,
        };
        res.json({err:null, profile_data:profile_information});
    });
    }catch (e) {
        res.json({err:e, profile_data:null});
        return;
    }
});



router.get('/get_profile_information_secure',function (req,res,next) {
    try{

        var pid = req.queryString("pid");
        var authkey = req.queryString("authkey");

        if(!req.session.user_data && authkey !== CFG.getConfig().settings_change_auth_key){
            res.json({err:true, status:"err_authkey_false_or_invalid_session"});
            return;
        }


        if(!pid || pid === ""){
            res.json({err:"pid not set",profile_data:null});
            return;
        }
        //FETCH PROFILE DATA
        profile_handling.get_profile_virtual_id(pid,function (lag_err,lag_res){
            if(lag_err || !lag_res){
                res.json({err:lag_err, profile_data:null});
            }
            res.json({err:null, profile_data:lag_res});
        });
    }catch (e) {
        res.json({err:e, profile_data:null});
        return;
    }
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
 - /rest/get_player_state?hwid=1334&sid=1234 -> {"err":null,"status":"ok","matchmaking_state":null,"game_state":null}
 */
router.get('/get_player_state',function (req,res,next) {
    try{
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
    }catch (e) {
        res.json({err:e, status:null, matchmaking_state:null,game_state:null});
        return;
    }
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
    try{
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
                res.json({err:psc_err, status:"ok"});
            });
        }else{
            //res.status(500);
            res.json({err:cvs_err, status:"err_session_key_sid_check_failed"});
        }
    });
    }catch (e) {
        res.json({err:e, status:null});
    }
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
    try{
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
    }catch (e) {
        res.json({err:e, status:null});
    }
});


router.get('/gmm',function (req,res,next) {
    try{

    CBL.get_board(CBL.get_start_opening_fen(),CBL.get_start_opening_fen(),CBL.PLAYER_TURN.BLACK,false,function (_err,_res) {
        res.json({err:_err,res:_res});
    });
    }catch (e) {
        res.json({err:e,res:null});
    }
});



router.get('/get_player_list',function (req,res,next) {
    try{


        res.json({err:_err,playerlist_ai:[{name:"ai",virtual_pid:"123",registered_ts:Date.now()}],playerlist_table:[{name:"table",virtual_pid:"123",registered_ts:Date.now()}],playerlist_virt:[{name:"virtual",virtual_pid:"123",registered_ts:Date.now()}]});

    }catch (e) {
        res.json({err:e});
    }
});




router.get('/server_config',function (req,res,next) {
    try{
        var authkey = req.queryString("authkey");
        var key = req.queryString("key");
        var value = req.queryString("value");

        if(!key || !value){
            //res.status(500);
            res.json({err:true, status:"err_query_paramter_value"});
            return;
        }
        //CHECK SESSION OR API KEY
        if(!req.session.user_data && authkey !== CFG.getConfig().settings_change_auth_key){
            res.json({err:true, status:"err_authkey_false_or_invalid_session"});
            return;
        }

        if(key === "settings_change_auth_key" || key === "settings_change_pw"|| key === "secret_addition"){
            res.json({err:true, status:"err_cant_set_this_key_over_webconsole"});
            return;
        }

        CFG.set_key(key,value);
        var cfg = CFG.getConfig();
        res.json({err:null,res:cfg});
    }catch (e) {
        res.json({err:e,res:null});
    }
});

router.get('/get_server_config',function (req,res,next) {
    try{
        //CHECK SESSION OR API KEY
        if(!req.session.user_data && req.queryString("authkey") !== CFG.getConfig().settings_change_auth_key){
            res.json({err:true, status:"err_authkey_false_or_invalid_session"});
            return;
        }
        //LOAD CONFIG AND SEND JSON
        var cfg = CFG.getConfig();
        var cfg_readonly = Object.assign({}, cfg); //CREATE A READ ONLY COPY OF THE CONFIG
        //DELETE SOME KEYS
        //TODO USE PREFIX IN KEYS TO REMOVE OR SPLIT CONFIG
        delete cfg_readonly.settings_change_auth_key;
        delete cfg_readonly.settings_change_pw;
        delete cfg_readonly.secret_addition;
        delete cfg_readonly.mongodb_connection_url;
        delete cfg_readonly.redis_connection_url;
        delete cfg_readonly.chess_move_validator_api_url;
        delete cfg_readonly.mongodb_database_name;
        delete cfg_readonly.mongodb_collection_profiles;
        delete cfg_readonly.mongodb_collection_games;
        delete cfg_readonly.mongodb_collection_lobby;
        delete cfg_readonly.webserver_default_port;
        delete cfg_readonly.app_name;

        res.json({err:null,res:cfg_readonly});
    }catch (e) {
        res.json({err:e,res:null});
    }
});


//------ SESSION MANAGEMENT --------- //
router.get('/check_login_state', function (req, res) {
    sess = req.session;
    if(sess.user_data){
        res.json({login_state:true});
    }else{
        res.json({login_state:false});
    }

});

router.post('/perform_user_login', function (req, res) {

    sess = req.session;
    var user_pw = req.body.user_pw;
    var origin_url = req.body.origin_url;



    //NOW CHECK PASSWORD
    //var p2 =  bcrypt.hashSync(user_pw,12);
    bcrypt.compare(sanitizer.sanitize(user_pw),CFG.getConfig().settings_change_pw, function (err, crypr_res) {
        if (crypr_res) {
            //APPLY USER DATA
            sess.user_data = {login_ts: Date.now()};
            //REDIRECT USER IF ORIGIN SET
            if(origin_url != null && origin_url !== ""){
                res.redirect(sanitizer.sanitize(origin_url));
            }else{
                res.redirect("/");
            }
        }else{
            res.redirect("/?login_invalid=1");
        }
    });
});

router.get('/get_user_config',function (req,res,next) {
    try{
        var hwid = req.queryString("hwid");
        var sid = req.queryString("sid");


        //CHECK INPUT PARAMETER
        if(!hwid || !sid ){
            //res.status(500);
            res.json({err:true, status:"err_query_paramter_hwid_or_sid_or_not_set", cfg:null});
            return;
        }

        //CHECK_SESSION
        session_handling.check_valid_session(hwid,sid,function (cvs_err,cvs_res) {
            if(cvs_err){
                //res.status(500);
                res.json({err:cvs_err, status:"err_session_check_failed",cfg:null});
                return;
            }
            //IF SESSION VALID -> LIST PLAYERS
            if(cvs_res){
                profile_handling.get_player_config(hwid,function (spc_err,spc_res) {
                    res.json({err:spc_err, status:"ok",cfg:spc_res});
                });
            }else{
                res.json({err:cvs_err, status:"err_cfg_load_failed",cfg:null});
            }
        });
    }catch (e) {
        res.json({err:e, status:null});
    }
});

router.post('/set_user_config',function (req,res,next) {
    try{
        var hwid = req.queryString("hwid");
        var sid = req.queryString("sid");
        var config_json = req.bodyJson();

    //    console.log(req);
        //CHECK INPUT PARAMETER
        if(!hwid || !sid ){
            //res.status(500);
            res.json({err:true, status:"err_query_paramter_hwid_or_sid_or_not_set"});
            return;
        }
        if(config_json == undefined || config_json.SETTINGS === null || config_json.USER_DATA === null){
            res.json({err:true, status:"err_bodyJson() is null"});
            return;
        }

        //CHECK_SESSION
        session_handling.check_valid_session(hwid,sid,function (cvs_err,cvs_res) {
            if(cvs_err){
                //res.status(500);
                res.json({err:cvs_err, status:"err_session_check_failed"});
                return;
            }
            //IF SESSION VALID -> LIST PLAYERS
            if(cvs_res){

                profile_handling.set_player_config(config_json,hwid,function (spc_err,spc_res) {
                    res.json({err:spc_err, status:spc_res});
                });
            }else{
                res.json({err:cvs_err, status:"err_cfg_update_failed"});
            }
        });
    }catch (e) {
        res.json({err:e, status:null});
    }
});

router.post('/store_user_log',function (req,res,next) {
    try{

        var hwid = req.queryString("hwid");
        var sid = req.queryString("sid");
        var log = req.bodyJson();

       // console.log(req);
        //CHECK INPUT PARAMETER
        if(!hwid || !sid ){
            //res.status(500);
            res.json({err:true, status:"err_query_paramter_hwid_or_sid_or_not_set"});
            return;
        }
        if(log == undefined || log.log === null){
            res.json({err:true, status:"err_bodyJson() is null"});
            return;
        }

        //CHECK_SESSION
        session_handling.check_valid_session(hwid,sid,function (cvs_err,cvs_res) {
            if(cvs_err){
                //res.status(500);
                res.json({err:cvs_err, status:"err_session_check_failed"});
                return;
            }
            //IF SESSION VALID -> LIST PLAYERS
            if(true || cvs_res){
                profile_handling.append_player_log(log,hwid,function (spc_err,spc_res) {
                    res.json({err:spc_err, status:spc_res});
                });
            }else{
                res.json({err:cvs_err, status:"err_cfg_update_failed"});
            }
        });
    }catch (e) {
        res.json({err:e, status:null});
    }
});


router.get('/set_user_config_single_key',function (req,res,next) {
    try{
        var authkey = req.queryString("authkey");
        var key = req.queryString("key");
        var value = req.queryString("value");
        var vid = req.queryString("vid");
        if(!key || !value || !vid){
            //res.status(500);
            res.json({err:true, status:"err_query_paramter_value"});
            return;
        }
        //CHECK SESSION OR API KEY
        if(!req.session.user_data && authkey !== CFG.getConfig().settings_change_auth_key){
            res.json({err:true, status:"err_authkey_false_or_invalid_session"});
            return;
        }



        profile_handling.set_player_user_config_key(key,value,vid,function (sp_err,sp_res) {
            res.json({err:sp_err, status:sp_res});
        });


    }catch (e) {
        res.json({err:e,res:null});
    }
});


module.exports = router;
