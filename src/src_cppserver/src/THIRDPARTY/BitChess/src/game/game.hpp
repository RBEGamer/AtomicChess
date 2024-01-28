/*
 * game.h
 *
 *  Created on: 24 Sep 2015
 *      Author: miles
 */

#ifndef GAME_GAME_HPP_
#define GAME_GAME_HPP_

#include "evaluation/search.hpp"
#include "position/position.hpp"
#include "position/move.hpp"

namespace bitchess {

class Game {
public:

	/**
	 * Constructs a Game instance with no polling function set.
	 * The Game will not check the polling function.
	 */
	Game() :
			// initialise polling function pointer to NULL
			pfunc() {
	}

	/**
	 * Constructs a Game instance with the given polling function. This function
	 * will be called regularly e.g. to check for input in a higher class.
	 * @param polling_function Function pointer for a polling function.
	 */
	Game( void (*polling_function)() ) :
			// initialise polling function pointer to given function
			pfunc(&polling_function) {
	}

	/**
	 * Gets the engine's next best selected move.
	 * @return a legal Move that the engine has deemed optional
	 */
	Move get_next_move();

private:
	//pointer to a polling function
	void (*pfunc)();

	//current board position
	Position current_position;

	//a searcher used to find the next move
	Search searcher;

};

}

#endif /* GAME_GAME_HPP_ */
