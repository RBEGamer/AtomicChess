import chess
import logging
import re

from flask import Flask, redirect, request, jsonify
import json  # FOR THE REST API
import os
app = Flask(__name__)

@app.route('/', methods=['get'])
def index():
    return redirect('/rest/state',code=302)


@app.route('/rest/state', methods=['get'])
def state():
    return jsonify({'status':'running'})

@app.route('/rest/validate_board', methods=['post'])
def validate_board():
    rq = request
    is_board_valid = True
    legal_mv = []
    legal_mv_coonverted = []
    is_go = False
    try:
        rq = request
        fen = rq.form['fen']
        board = chess.Board(fen)


        #NOW CHECK BOARD STATUS
        if board.status() == chess.Status.VALID:
            is_board_valid = True
        else:
            is_board_valid = False

        legal_mv = list(board.pseudo_legal_moves)
        is_go = board.is_game_over() or board.is_checkmate()
        err = None


        for mv in legal_mv:
            legal_mv_coonverted.append(mv.uci())
        print(board)
    except Exception as e:
        logging.warning('/rest/validate_board')
        logging.warning(fen)
        logging.warning(board)
        logging.warning(e)
        is_board_valid = False
        err = e
    return jsonify({'err': err, 'is_board_valid': is_board_valid,'legal_moves': legal_mv_coonverted, 'is_game_over': is_go})

@app.route('/rest/check_move', methods=['post'])
def check_move():
    rq = request
    err = None
    is_valid = False
    try:
        #GET THE POST REGQUEST FORM DATA
        fen = rq.form['fen']
        move = rq.form['move']
        use_pseudo_legal_moves = rq.form['legal_move_settings']



        #CHECK THE PATTERN OF THE MOVE VIA REGEX FROM->TO FIELD
        move = str(move).lower()
        mv_patter = re.compile("^([a-h][1-8]){2}[a-zA-Z]*$")
        if not re.match(mv_patter, move):
            return jsonify({'err': 'move_pattern doesnt match', 'is_valid': is_valid, 'move': move, 'fen': fen})
        #GENERATE A BOARD FROM THE FEN CODE
        board = chess.Board(fen)# GENERATE A BOARD
        print(board)

        #GENERATE THE LEGAL MOVES TO CHECK AGAINST THE GIVEN MOVE
        legal_mv = []
        if (str(use_pseudo_legal_moves) == "1"):
            legal_mv = list(board.pseudo_legal_moves)
        else:
            legal_mv = list(board.legal_moves)

        err = None
        #NOW CHECK ALL LEGAL MOVES WITH THE EXISTING MOVE
        for mv in legal_mv:
           if  move in mv.uci():
            is_valid = True
            break
        print(board)

    except Exception as e:
        logging.warning('/rest/check_move')
        logging.warning(fen)
        logging.warning(move)
        logging.warning(e)
        is_valid = False
        err = e
    return jsonify({'err': err, 'is_valid': is_valid, 'move': move, 'fen': fen})

@app.route('/rest/execute_move', methods=['post'])
def execute_move():
    err = None
    rq = request

    try:
        fen = rq.form['fen']
        move = rq.form['move']
        use_pseudo_legal_moves = rq.form['legal_move_settings']

        move = str(move).lower()
        # GENERATE A BOARD FROM THE FEN CODE
        board = chess.Board(fen)  # GENERATE A BOARD
        print(board)

        # CHECK THE PATTERN OF THE MOVE VIA REGEX FROM->TO FIELD
        move = str(move).lower()
        mv_patter = re.compile("^([a-h][1-8]){2}[a-zA-Z]*$")
        if not re.match(mv_patter, move):
            return jsonify({'err': 'move_pattern doesnt match', 'new_fen': '', 'move_executed': 0})


        mv_list = []
        if(str(use_pseudo_legal_moves) == "1"):
            mv_list = board.pseudo_legal_moves
        else:
            mv_list = board.legal_moves

        if chess.Move.from_uci(move) not in mv_list:
            return jsonify({'err': 'move not valid', 'new_fen': '', 'move_executed': 0, 'next_player_turn':0})
        board.push(chess.Move.from_uci(move))
        #TODO CHECK UCI MOVE STRING
        #TODO PUSH MOVE
        # RETURN NEW BOARD PLAYER
        #RETURN DRAWS!
    except Exception as e:
        err = e
        logging.warning('/rest/execute_move')
        logging.warning(e)
        return jsonify({'err': err, 'new_fen': "", 'move_executed': 0, 'next_player_turn':0})

    return jsonify({'err': None, 'new_fen': board.fen(), 'move_executed': True, 'next_player_turn': int(not board.turn)})

@app.route('/rest/init_board', methods=['get'])
def init_board():
    err = None
    rq = request

    try:
        board = chess.Board()
        print(board)
    except Exception as e:
        err = e
        logging.warning('/rest/init_board')
        logging.warning(e)
        return jsonify({'err': err, 'board': ""})

    return jsonify({'err': None, 'board': board.fen()})




# TODO RATERATE BOARD ->for each player rating
# CALL FROM BACKEND IF CHECH GAME FINISHED
# PAWN=1, KNIGHT=3,# BISHOP=3, ROOK=5, QUEEN=9
@app.route('/rest/get_player_score', methods=['post'])
def get_player_score():
    return jsonify({'err':None, 'score_white_player':None,'score_black_player':None})

if __name__ == '__main__':
    logging.basicConfig(filename='./ATC_Move_Validator.log', level=logging.DEBUG)
    logging.info('ATC_Move_Validator - started internal 5001')
    app.run(host='0.0.0.0', port=5001)

