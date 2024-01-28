/*
 * search.h
 *
 *  Created on: 24 Sep 2015
 *      Author: miles
 */

#ifndef GAME_EVALUATION_SEARCH_HPP_
#define GAME_EVALUATION_SEARCH_HPP_

#include "../../util/util.hpp"
#include "evaluator.hpp"


namespace bitchess {

/**
 * The Search class searches for the most optimal move to play next.
 */
class Search {
public:
	/**
	 * Returns the best move found so far.
	 * @param position Current board position.
	 * @param side_to_move Side to find a move for.
	 * @return the best Move.
	 */
	Move get_next_move(Position position, Colour side_to_move);

private:
	Evaluator evaluator;
};

}

#endif /* GAME_EVALUATION_SEARCH_HPP_ */
