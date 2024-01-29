/*
 * evaluator.h
 *
 *  Created on: 24 Sep 2015
 *      Author: miles
 */

#ifndef GAME_EVALUATION_EVALUATOR_HPP_
#define GAME_EVALUATION_EVALUATOR_HPP_


namespace bitchess {

/**
 * The Evaluator evaluates a given position to assess the chances of the
 * side to move of eventually winning the game.
 */
class Evaluator {
public:
	/**
	 * Evaluates the relative chance (in centipawns) that the side to move will prevail.
	 * Positive scores favour the side to move, negative scores favour the opponent.
	 * @param position Position to evaluate.
	 * @param side_to_move Side with the next move.
	 * @return a floating point evaluation.
	 */
	static float evaluate(Position position, Colour side_to_move);
};

}

#endif /* GAME_EVALUATION_EVALUATOR_HPP_ */
