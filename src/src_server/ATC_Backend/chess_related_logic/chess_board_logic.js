var request = require('request');
var CONFIG = require('../config'); //include the cofnig file

const CHESS_FIGURES = Object.freeze({
    BLACK:{
        PAWN:"p",
        KNIGHT:"n",
        BISHOP:"b",
        ROOK:"r",
        QUEEN:"q",
        KING:"k"
    },WHITE:{
        PAWN:"P",
        KNIGHT:"N",
        BISHOP:"B",
        ROOK:"R",
        QUEEN:"Q",
        KING:"K"
    }
});

const PLAYER_TURN = Object.freeze({
    WHITE: 0,
    BLACK: 1,
    NONE: 2
});

function get_player_score(_extendet_fen,_callback){
    _callback(null, 1000);
    //TODO USE MVALIDATOR THE CALCULATE POINTS
}


function check_board_and_get_legal_moves(_extendet_fen,_callback){
    request.post({url:CONFIG.getConfig().chess_move_validator_api_url+"/rest/validate_board", formData: {fen:_extendet_fen}}, function optionalCallback(err, httpResponse, body) {
        if (err) {
            _callback(err,null);
            return;
        }
        //console.log('Upload successful!  Server responded with:', body);
        var jsonbody = null;
        try {
             jsonbody = JSON.parse(body);
             if(!jsonbody || jsonbody.legal_moves == null || jsonbody.is_board_valid == null || jsonbody.is_game_over == null){
                 console.error(jsonbody);
                 throw "json result invalid";
             }
        }catch (e) {
            console.error(e);
            _callback(e,null,[]);
            return;
        }
        _callback(err, jsonbody.is_board_valid, jsonbody.legal_moves,jsonbody.is_game_over);
    });
}
//REST API IF PLAYER HAS STATE THAT THE HAS TO MAKE A MOVE ->check the suggetsted move then use a api to make a move
//return the new board save it in the database
//the other player polls the current board via the status
function execute_move(_board,_move, _callback){
    if(!_board || !_board.ExtendetFen){
        _callback("!_board.ExtendetFen",false,null,false);
        return;
    }
    request.post({url:CONFIG.getConfig().chess_move_validator_api_url+"/rest/execute_move", formData: {fen:_board.ExtendetFen,move:_move}}, function optionalCallback(err, httpResponse, body) {
        if (err) {
            _callback(err,null);
            return;
        }

        var jsonbody = null;
        try {
            jsonbody = JSON.parse(body);
            //TODO
            if(!jsonbody || jsonbody.move_executed == null || jsonbody.new_fen == null || jsonbody.next_player_turn == null){
                console.error(jsonbody);
                throw "json result invalid";
            }
            //IF ERROR IN RESPONSE THROW IT
            if(jsonbody.err){
                throw jsonbody.err
            }
        }catch (e) {
            console.error(e);
            _callback(e,null, null,null);
            return;
        }
        _callback(err, jsonbody.move_executed, jsonbody.new_fen,jsonbody.next_player_turn);
    });
}





module.exports = {
    get_start_opening_fen: function () {
    return "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";
    },

    get_board: function (_fen,_pturn,_move, _is_initial_board,_turn_count,_callback) {

        //BUILD EXTENDET FEN SIGNATURE
        var pturn_fen = "w";
        if(_pturn){
            pturn_fen = "b";
        }
        var tc = _turn_count;
        if(_is_initial_board){
            _turn_count = 1;
        }
        var ext_fen =  _fen + " " + pturn_fen + " - " + "- " + "0 " + String(tc);

        check_board_and_get_legal_moves(ext_fen,function (cb_err,cb_is_board_valid,cb_legal_moves,cb_is_game_over) {

            _callback(cb_err,{
                fen:_fen,
                ExtendetFen:ext_fen,
                player_turn:_pturn,
                move:_move,
                initial_board:_is_initial_board,
                is_board_valid:cb_is_board_valid,
                legal_moves:cb_legal_moves,
                is_game_over:cb_is_game_over
            });
        })

    },
    get_fen_from_board:function (_board) {
        return {}
    },
    execute_move,
    get_player_score,
    CHESS_FIGURES,
    PLAYER_TURN,
    check_board_and_get_legal_moves
};