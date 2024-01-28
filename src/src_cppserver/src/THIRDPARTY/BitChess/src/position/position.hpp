/*
 * position.h
 *
 *  Created on: 24 Sep 2015
 *      Author: miles
 */

#ifndef POSITION_POSITION_HPP_
#define POSITION_POSITION_HPP_

#include <map>
#include <array>
#include <vector>

namespace bitchess {

/**
 * Represents a board position in chess, with piece locations and other metadata
 * such as player castling rights.
 */
class Position {
public:
	/**
	 * Initialises an empty board.
	 */
	Position();
	/**
	 * Initialises the position with the given Forsyth-Edwards notation string
	 * (https://en.wikipedia.org/wiki/Forsyth%E2%80%93Edwards_Notation).
	 * @param FEN_string Forsyth-Edwards notation string, e.g.
	 * "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1" for the starting position.
	 */
	Position(std::string FEN_string);

	/**
	 * Counts pieces on the board by colour.
	 * @return a map organised by colour and piece type, e.g. in the starting position
	 * get_piece_counts()[white][king] = 1, get_piece_counts()[black][pawn] = 8.
	 */
	std::map<Colour,std::map<PieceType, int>> get_piece_counts();

	/**
	 * Gets a list of legal moves that can be played by the side_to_move in this position.
	 * @param side_to_move Side currently on move.
	 * @return a vector of legal moves.
	 */
	std::vector<Move> get_all_legal_moves(Colour side_to_move);


private:
	/**
	 * Tests each moves for illegal check positions and removes any from the list that are illegal.
	 * @param side_to_move Side currently on move.
	 * @param move_list a vector of pseudolegal moves generated from this position. Will be modified
	 * with illegal moves removed.
	 * @return the modified move_list
	 */
	std::vector<Move> test_checks(Colour side_to_move, std::vector<Move> &move_list);

	std::map<Colour, std::map<PieceType,pieces::Piece>> map_pieces; //a map of the pieces
	std::array<*pieces::Piece,64> array_pieces; //an array representing board squares of the pieces.

};

}

#endif /* POSITION_POSITION_HPP_ */
