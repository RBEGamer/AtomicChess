/*
 * xboard.h
 *
 *  Created on: 24 Sep 2015
 *      Author: miles
 */

#ifndef GAME_XBOARD_HPP_
#define GAME_XBOARD_HPP_

#include <iostream>
#include <string>

namespace bitchess {

/**
 * Communicates with an XBoard based chess GUI through the input and output streams
 *  given (by default the stdin and stdout streams), controlling the underlying Game
 *  class based on commands given. Reference for protocol:
 *  http://www.gnu.org/software/xboard/engine-intf.html
 */
class XBoardCtrl {
public:
	/**
	 * Constructs an XBoardCtrl instance with the specified input and output streams.
	 */
	XBoardCtrl( std::istream in, std::ostream out );
	/**
	 * Constructs an XBoardCtrl instance using stdin and stdout
	 */
	XBoardCtrl() :
			XBoardCtrl(std::cin, std::cout) {
	}

	/**
	 * Polls the input stream given for input. If input is detected, the class waits for
	 * a \n to be sent, then sends the whole string to parsecommand(), otherwise control
	 * is passed back to the calling function.
	 */
	void poll();

	/**
	 * Parses the command given. The string is split by spaces. The first portion corresponds
	 * to the command, and if valid, an appropriate action is taken. Other portions are parsed
	 * as parameters. E.g. "command p1 p2 p3" would be split into string command="command" and
	 * parameters=vector<string> {"p1", "p2", "p3"}.
	 * @param commandstr Command to parse.
	 */
	void parse_command( std::string commandstr );
};

}

#endif /* GAME_XBOARD_HPP_ */
