var CFG = require('../config/config');
var MDB = require('./mongo_db_connection'); //include mongo db connection
var LH = require("../session_handling/lobby_handling");
var UUID = require('uuid');
var HELPER_FUNCTIONS = require("./helper_functions");
var CBL = require("../chess_related_logic/chess_board_logic");
var PH = require("./profile_handling");

//game cleanup job wenn spiel last interaction <60m
const GAME_STATE = Object.freeze({ //TODO UPPER CASE
    init: 0, //TODO ----------------------- REMOVE -----------------------------
    waiting_player_setup_confirmation: 1, //WAIT FOR BOTH PLAYERS TO GET THEIR BOARD READY
    PLAYER_WHITE_TURN: 8,
    PLAYER_BLACK_TURN: 9,
    PLAYER_BLACK_WON: 10,
    PLAYER_WHITE_WON: 11,
    aborted_game: 12
});

const PLAYER_TURN_STATE = Object.freeze({
    NONE: 0,
    PLAYER_SETUP_CONFIRMED: 1,
    IDLE: 2, //TODO --------------- REMOVE ----------------
    WAITING_FOR_MOVE: 3,
    MOVE_FINISHED: 4,
});

function check_player_state_to_determ_new_game_state(_game_id, _callback) {
    //danach prüfe den status beider spielr und starte das spiel mit WAITING_FOR MOVE
    console.log("---------- CHECKING GAME STATE ------------");
    //GET GAME FROM DATABASE
    MDB.getGameCollection().findOne({id: _game_id, DOCTYPE: "GAME"}, function (err, res) {
        if (err) {
            _callback(err, null);
            return;
        }
        var update_query = res;
        //EVERY PLAYER HAS COMPLETED HIS OWN SETUP; NO START THE GAME WITH THE WHITE PLAYER
        if (res.game_state === GAME_STATE.waiting_player_setup_confirmation && res.player_white_state === PLAYER_TURN_STATE.PLAYER_SETUP_CONFIRMED && res.player_black_state === PLAYER_TURN_STATE.PLAYER_SETUP_CONFIRMED) {
            console.log("-- GAME CONFIRMED -- STARTING GAME THE RIGHT WAY ---");
            // update_query["current_board"] = {};
            if (res.current_board.player_turn === 0) {
                update_query["game_state"] = GAME_STATE.PLAYER_WHITE_TURN;
                update_query["current_board"].player_turn = CBL.PLAYER_TURN.WHITE;
            } else {
                update_query["game_state"] = GAME_STATE.PLAYER_BLACK_TURN;
                update_query["current_board"].player_turn = CBL.PLAYER_TURN.BLACK;
            }
            update_query["current_board"].initial_board = false;
        }

        //CHECK IF BLACK PLAYER HAS WON
        //THE PLAYER COLOR IS INVERTED BEACUSE THE CONTAINS THE NEXT PLAYER ALREADY
        if (res.current_board.is_game_over && res.game_state !== GAME_STATE.aborted_game) {
            console.info("------ GAME OVER -----");
            update_query["game_state"] = GAME_STATE.aborted_game;
            update_query["player_won"] = res.current_board.player_turn;
            //GET THE PLAYER WON HWID TO APPLY POINT TO THEIR PROFILE
            var player_won_hwid = null;
            if (res.current_board.player_turn === CBL.PLAYER_TURN.WHITE) {
                player_won_hwid = res.player_black_hwid;
                update_query["player_won"] = res.current_board.PLAYER_WHITE_WON;
            } else if (res.current_board.player_turn === CBL.PLAYER_TURN.BLACK) {
                player_won_hwid = res.player_white_hwid;
                update_query["player_won"] = res.current_board.PLAYER_BLACK_WON;
            }
            //APPLY_POINTS TO USER PROFILE
            CBL.get_player_score(res.current_board.ExtendetFen, function (gpp_err, gpp_res) {
                //LESS TURNS MORE POINTS
                if (res.turn_history.length > 0) {
                    gpp_res *= (1.0 / res.turn_history.length) * 100.0;
                }
                //LESS TIME NEEDED MORE POINTS
                //gpp_res *= (1.0 / ((Date.now()-res.game_init_timestamp)*0.1))*10000.0;
                //SAVE POINTS IN PROFILE

                PH.apply_points_to_profile(gpp_res, _game_id, player_won_hwid, function (ap_err, ap_res) {
                    //SET PLAYER STATE FOR EACH PLAYER
                    LH.set_player_lobby_state(res.player_black_hwid, LH.player_state.idle, function (spb_err, spb_res) {
                        LH.set_player_lobby_state(res.player_white_hwid, LH.player_state.idle, function (spw_err, spw_res) {
                            MDB.getGameCollection().updateOne({id: _game_id}, {$set: update_query}, function (uo_err, uo_res) {
                                _callback(uo_err, "ok");
                                return;
                            });
                        });
                    });
                });
            });
            //TODO RESET CLIENTS IN GAME STATE !!!!!

            console.info("------ GAME OVER -----");
            return;
        }


        //UPDATE GAME ENTRY IF NEEDED
        if (update_query) {
            MDB.getGameCollection().updateOne({id: _game_id}, {$set: update_query}, function (uo_err, uo_res) {
                _callback(uo_err, "ok");
                return;
            });
        } else {
            _callback(null, "ok_nothing_to_do_here");
            return;
        }

    });


}


//TODO CHECK IF A GAME WITH THESE PLAYERS IS ACTIVE CURRENTLY -> ABORT THIS GAME
//TODO ABBORT GAME FUNCTION
//TODO SET PLAYER LOBBY STATE TO IDLE FOR BOTH PLAYER
// RUN AT LOGIN
function cancel_match_for_player(_hwid, _callback) {
    if (_hwid == undefined || !_hwid || _hwid === "") {
        _callback("_hwid not set", null);
        return;
    }
    //GET CURRENT ACTIVE GAME FOR PLAYEWR WITH _HWID
    get_player_active_game_state(_hwid, function (gpagA_err, gpagA_res, gpagA_res_simple) {
        if (gpagA_err || !gpagA_res) {
            _callback(gpagA_err, null);
            return;
        }
        //GET THE OTHER PLAYER ID
        var other_player = null;
        if (_hwid !== gpagA_res.player_black_hwid) {
            other_player = gpagA_res.player_black_hwid;
        } else {
            other_player = gpagA_res.player_white_hwid;
        }

        LH.set_player_lobby_state(_hwid, LH.player_state.idle, function (splsA_err, splsA_res) {
            LH.set_player_lobby_state(other_player, LH.player_state.idle, function (splsB_err, splsB_res) {
                MDB.getGameCollection().updateOne({
                    id: gpagA_res.id,
                    DOCTYPE: "GAME"
                }, {$set: {game_state: GAME_STATE.aborted_game}}, function (uo_err, uo_res) {
                    _callback(null, {"state": "ok"});
                });
            });
        });

    });
    //SET NEW GAME STATE
    //SET OWN PLAYER STATE TO IDLE
    //SET OTHER PLAYER STATE TO IDLE


}


function make_move(_hwid, _move, _callback) {

    console.log("make_move :" + String(_hwid) + " -> " + String(_move));
    //TODO get game
    get_player_active_game(_hwid, function (gag_err, gag_res) {
        if (gag_err || !gag_res.current_board) {
            _callback(gag_err, "no_active_game_found");
            return;
        }
        //CHECK IF PLAYER IS TURN EBALED
        var player_is_on_turn = false;
        if (gag_res.current_board.player_turn === CBL.PLAYER_TURN.WHITE && gag_res.player_white_hwid === _hwid) {
            player_is_on_turn = true;
        } else if (gag_res.current_board.player_turn === CBL.PLAYER_TURN.BLACK && gag_res.player_black_hwid === _hwid) {
            player_is_on_turn = true;
        } else {
            _callback(true, "turn_for_this_player_is_not_enabled");
            return;
        }

        CBL.execute_move(gag_res.current_board, _move, function (em_err, em_move_executed, em_new_fen, em_next_player_turn) {
            if (em_err) {
                _callback(true, em_err);
                return;
            }
            if (!em_move_executed) {
                _callback(true, "move_execution_failed");
                return;
            }
            //CREATE A COPY OF THE BOARD OBJECT
            //JS OBJECT ARE USING REFERENCES IF YOU DOES NOT CLONE IT
            var copy_board = Object.assign({}, gag_res.current_board);
            copy_board.fen = em_new_fen;
            copy_board.ExtendetFen = em_new_fen;
            copy_board.player_turn = em_next_player_turn;
            copy_board.move = _move;
            copy_board.initial_board = false;
            //REQUEST NEW INFORMATION ABOUT THE NEW BOARD
            //EG THE CHESSMATE STATE OR THE NEW LEGAL MOVES THE NEXT PLAYER CAN MAKE
            CBL.check_board_and_get_legal_moves(em_new_fen, function (cb_err, cb_is_board_valid, cb_legal_moves, cb_is_game_over) {
                if (cb_err) {
                    _callback(true, cb_err);
                    return;
                }
                copy_board.is_board_valid = cb_is_board_valid;
                copy_board.legal_moves = cb_legal_moves;
                copy_board.is_game_over = cb_is_game_over;
                //UPDATE LAST INTERACTION TIMESTAMP
                gag_res.last_game_interaction = Date.now();
                //UPDATE PLAYER STATES

                //UPDATE GAME STATE
                if (em_next_player_turn === CBL.PLAYER_TURN.WHITE) {
                    gag_res.game_state = GAME_STATE.PLAYER_WHITE_TURN;
                } else {
                    gag_res.game_state = GAME_STATE.PLAYER_BLACK_TURN;
                }
                //SAVE THE NEW BOARD IN THE LOG HISTORY
                gag_res.turn_history.push(copy_board);
                gag_res.current_board = copy_board;
                //FINALLY WRITE THE NEW BOARD TO THE DATABASE
                MDB.getGameCollection().updateOne({id: gag_res.id}, {$set: gag_res}, function (uo_err, uo_res) {
                    if (uo_err) {
                        _callback(uo_err, null);
                        return
                    }
                    check_player_state_to_determ_new_game_state(gag_res.id, function (dns_err, dns_res) {
                        _callback(dns_err, dns_res);

                    });
                });
            });
        });
    });
}

//TODO ALSO USE IN CHECK ACTIVE PLAER
//TODO ALSO USE IN UPDATE
//TODO ALSO USE IN get_player_active_game_state
function get_player_active_game(_hwid, _callback) {
    MDB.getGameCollection().findOne({
        game_state: {$not: {$eq: GAME_STATE.aborted_game}},
        $or: [{player_white_hwid: _hwid}, {player_black_hwid: _hwid}],
        DOCTYPE: "GAME"
    }, function (err, res) {
        if (err) {
            _callback(err, false);
            return;
        }
        if (res) {
            _callback(null, res);
            return;
        } else {
            _callback(null, null);
            return;
        }
    });
}

//IF ONE PLAYER HA A ACTIVE GAME THE FUNCTIONS RETUNRS TRUE
function check_active_player_game(_hwid_player_one, _hwid_player_two, _callback) {
    MDB.getGameCollection().findOne({
        game_state: {$not: {$eq: GAME_STATE.aborted_game}},
        $or: [{player_white_hwid: _hwid_player_one}, {player_black_hwid: _hwid_player_two}, {player_white_hwid: _hwid_player_two}, {player_black_hwid: _hwid_player_one}],
        DOCTYPE: "GAME"
    }, function (err, res) {
        if (err) {
            _callback(err, false);
            return;
        }
        if (res) {
            _callback(null, true);
            return;
        } else {
            _callback(null, false);
            return;
        }
    });
}

//TODO EXTRA FUNCTION FOR UPDATING THE PLAYER STATE
function set_player_setup_confirmation(_hwid, _callback) {
    //GET THE GAME LINKED WITH THE PLAYER HWID TO UPDATE THE PLAYER STATE
    MDB.getGameCollection().findOne({
        game_state: {$not: {$eq: GAME_STATE.aborted_game}},
        $or: [{player_white_hwid: _hwid}, {player_black_hwid: _hwid}],
        DOCTYPE: "GAME"
    }, function (err, res) {
        if (err || !res) {
            _callback(err, null, null);
            return;
        }
        var im_white = false;
        if (res.player_white_hwid === _hwid) {
            im_white = true;
        }
        var update_query = null;
        //DETERMINE WHICH PLAYER SIDE SETUP CONFIRMATION HAS TO BE UPDATE
        if (im_white && res.player_white_state === PLAYER_TURN_STATE.NONE) {
            update_query = {
                $set: {player_white_state: PLAYER_TURN_STATE.PLAYER_SETUP_CONFIRMED}
            }
        } else if (!im_white && res.player_black_state === PLAYER_TURN_STATE.NONE) {
            update_query = {
                $set: {player_black_state: PLAYER_TURN_STATE.PLAYER_SETUP_CONFIRMED}
            }
        }
        //IF A PLAYER CONFIRMATION HAS TO UPDATE
        if (update_query) {
            //UPDATE THE ENTRY FOR THE SPECIFIC PLAYER
            MDB.getGameCollection().updateOne({_id: res._id}, update_query, function (uo_err, uo_res) {
                //DETERMINE THE NEW GAME STATE BASED ON THE PLAYERS GAME STATES
                check_player_state_to_determ_new_game_state(res.id, function (cps_err, cps_res) {
                    _callback(uo_err, null);
                });
            });
        } else {
            check_player_state_to_determ_new_game_state(res.id, function (cps_err, cps_res) {
                _callback(cps_err, null);
                return
            });
        }
    });
}


//GET FOR A PLAYER THE CURRENT ACTIVE GAME IF FOUND
//IT REUTRNS THE CURRENT BOARD; AND A SIMPLIFIED STATE WITH RE RELEVANT INFORMATION FOR THE PLAYER TABLE
function get_player_active_game_state(_hwid, _callback) {
    MDB.getGameCollection().findOne({
        game_state: {$not: {$eq: GAME_STATE.aborted_game}},
        $or: [{player_white_hwid: _hwid}, {player_black_hwid: _hwid}],
        DOCTYPE: "GAME"
    }, function (err, res) {
        if (err || !res) {
            _callback(err, null, null);
            return;
        }
        var im_white = false;
        if (res.player_white_hwid === _hwid) {
            im_white = true;
        }
        var is_my_turn = false;
        if (im_white && res.current_board.player_turn === CBL.PLAYER_TURN.WHITE) {
            is_my_turn = true;
        } else if (!im_white && res.current_board.player_turn === CBL.PLAYER_TURN.BLACK) {
            is_my_turn = true;
        }
        var is_syncing_phase = false;
        if (res.game_state === GAME_STATE.waiting_player_setup_confirmation) {
            is_syncing_phase = true;
        }
        var local_player_state = null;
        if (im_white) {
            local_player_state = res.player_white_state;
        } else {
            local_player_state = res.player_black_state;
        }

        var game_simplified = {
            current_board: res.current_board,
            game_state: res.game_state,
            im_white_player: im_white,
            is_my_turn: is_my_turn,
            is_syncing_phase: is_syncing_phase,
            player_state: local_player_state,
            playerprofiles: res.playerprofiles

        }
        _callback(null, res, game_simplified);
        return;
    });
}

function start_match(_player_a_hwid, _player_b_hwid, _callback) {
    console.log("-------- STARTING MATCH FOR --------------")
    console.log(_player_a_hwid);
    console.log(_player_b_hwid);
    if (_player_a_hwid === _player_b_hwid) {
        console.log("CANT START A PATCH WITH THE SAME PLAYER " + _player_a_hwid + " " + _player_b_hwid);
        _callback("CANT START A PATCH WITH THE SAME PLAYER");
        return;
    }


    //DO NOTHING IF A GAME IS RUNNING
    check_active_player_game(_player_a_hwid, _player_b_hwid, function (cap_err, cap_is_game_running) {
            if (cap_err || cap_is_game_running) {
                _callback("A game is alredy running for one of the players");
                return;
            }
            //GET THE PLAYER PROFILE INFORMATION FOR THE NAMES AND VIRTUAL IDS
            PH.get_profile(_player_a_hwid, function (_phgp_a_err, _phgp_a_res) {
                PH.get_profile(_player_b_hwid, function (_phgp_b_err, _phgp_b_res) {


                    // MARKING THE PLAYER AS INGAME
                    LH.set_player_lobby_state(_player_a_hwid, LH.player_state.preparing_ingame, function (lsa_err, lsa_res) {
                        if (lsa_err) {
                            console.error("player_a state switching failed");
                            return;
                        }
                        LH.set_player_lobby_state(_player_b_hwid, LH.player_state.preparing_ingame, function (lsb_err, lsb_res) {
                            //ON ERROR RESET THE STATE OF THE OTHER PLAYER
                            if (lsb_err) {
                                console.error("player_a state switching failed");
                                LH.set_player_lobby_state(_player_a_hwid, LH.player_state.idle, function () {
                                });
                                _callback(lsb_err, null);
                                return;
                            }
                            //WHICH PLAYER IS BLACK/WHITE
                            var rnd = HELPER_FUNCTIONS.randomInteger(0, 1); //GET A RANDOM MUMBER
                            var white_player = [_player_a_hwid, _player_b_hwid][rnd]; //ASSIGN A PLAYER ID TO THE RANDOM NUMBER TO DETERM BLACK OR WHITE
                            var black_player = [_player_b_hwid, _player_a_hwid][rnd];
                            //POPULATE THE PLAYER PROFILE INFORMATION TO SHOW ON THE WEBCLIENT THE NAME OF THE OTHER PLAYER
                            var white_player_profile = null;
                            if (_phgp_a_res && white_player === _phgp_a_res.hwid) {
                                white_player_profile = {
                                    firendly_name: _phgp_a_res.friendly_name,
                                    rank: _phgp_a_res.rank,
                                    virtual_player_id: _phgp_a_res.virtual_player_id,
                                    player_type: _phgp_b_res.player_type
                                }
                            } else if (_phgp_b_res && white_player === _phgp_b_res.hwid) {
                                white_player_profile = {
                                    firendly_name: _phgp_b_res.friendly_name,
                                    rank: _phgp_b_res.rank,
                                    virtual_player_id: _phgp_b_res.virtual_player_id,
                                    player_type: _phgp_b_res.player_type
                                }
                            }

                            var black_player_profile = null;
                            if (_phgp_a_res && black_player === _phgp_a_res.hwid) {
                                black_player_profile = {
                                    firendly_name: _phgp_a_res.friendly_name,
                                    rank: _phgp_a_res.rank,
                                    virtual_player_id: _phgp_a_res.virtual_player_id
                                }
                            } else if (_phgp_b_res && black_player === _phgp_b_res.hwid) {
                                black_player_profile = {
                                    firendly_name: _phgp_b_res.friendly_name,
                                    rank: _phgp_b_res.rank,
                                    virtual_player_id: _phgp_b_res.virtual_player_id
                                }
                            }
                            //POPULATE_PLAYER_PROFILE
                            var playerprofile_data = {
                                white_player: white_player_profile,
                                black_player: black_player_profile
                            };

                            //GET OUR STARTING POSITION IN THIS CASE FROM
                            //gsof_res CONTAINS THE START BOARD
                            CBL.get_start_opening_fen(function (gsof_err, gsof_res) {
                                //generate our first board, its an intialboard with player white starts
                                //this board will be used to sync the tables of the players
                                CBL.get_board(gsof_res, CBL.PLAYER_TURN.WHITE, null, true, 1, function (gb_err, gb_board) {
                                    //GET VARIABLE FOR GAME INIT
                                    var game_init_structure = {
                                        id: UUID.v1(),
                                        DOCTYPE: "GAME",
                                        player_white_hwid: white_player,
                                        player_black_hwid: black_player,
                                        player_white_state: PLAYER_TURN_STATE.NONE,
                                        player_black_state: PLAYER_TURN_STATE.NONE,
                                        player_won: null,
                                        game_init_timestamp: Date.now(),
                                        last_game_interaction: Date.now(),
                                        game_state: GAME_STATE.waiting_player_setup_confirmation,
                                        observer_player: [], //MAYBE FOR OBSERVER PLAYER -> FEATURES
                                        start_board_string: gsof_res, //GET A NORMAL OPENING STRING
                                        current_board: gb_board,                   //REPRESENT THE CURRENT BOARD
                                        current_active_player: white_player, //WHICH PLAYER IS ACTIVE AND CAN MAKE A TURN
                                        turn_history: [gb_board], //CONTAIN ALL MADE MOVES EG FOR REPLY
                                        playerprofiles: playerprofile_data //CONTAINS THE PLAYER PROFILE DATA => PLAYERNAME AND VIRTUAL ID
                                    }
                                    //WRITE TO DB THE NEW GAME CREATED
                                    MDB.getGameCollection().insertOne(game_init_structure, function (io_err, io_res) {
                                        _callback(io_err, io_res);
                                        return;
                                    });
                                });
                            });
                        });
                    });
                });
            });
        }
    )
    ;
}


function list_all_games(_callback) {
    MDB.getGameCollection().aggregate([{
        $lookup:
            {
                from: CFG.getConfig().mongodb_collection_profiles,
                localField: 'player_white_hwid',
                foreignField: 'hwid',
                as: 'profile_white'
            }
    }, {
        $lookup:
            {
                from: CFG.getConfig().mongodb_collection_profiles,
                localField: 'player_black_hwid',
                foreignField: 'hwid',
                as: 'profile_black'
            }
    }, {
        $match: {
            DOCTYPE: "GAME"
        }

    }]).toArray(function (err, res) {
        if (err) {
            _callback(err, null);
            return;
        }
        if (res) {
            _callback(null, res);
        } else {
            _callback(null, null);
        }
    });
}


function get_game(_game_id, _callback) {
    MDB.getGameCollection().aggregate([{
        $lookup:
            {
                from: CFG.getConfig().mongodb_collection_profiles,
                localField: 'player_white_hwid',
                foreignField: 'hwid',
                as: 'profile_white'
            }
    }, {
        $lookup:
            {
                from: CFG.getConfig().mongodb_collection_profiles,
                localField: 'player_black_hwid',
                foreignField: 'hwid',
                as: 'profile_black'
            }
    }, {
        $match: {
            DOCTYPE: "GAME",
            id: _game_id
        }

    }]).toArray(function (err, res) {
        if (err) {
            _callback(err, null);
            return;
        }
        if (res && res.length >= 1) {
            _callback(null, res[0]);
        } else {
            _callback(null, null);
        }
    });
}

module.exports = {
    start_match,
    get_player_active_game_state,
    set_player_setup_confirmation,
    make_move,
    cancel_match_for_player,
    list_all_games,
    get_game,
    //EXPOSED VARIABLED
    GAME_STATE,
    PLAYER_TURN_STATE
}
