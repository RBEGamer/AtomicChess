#BitChess

A simple but rapid C++ chess engine which generates moves given a particular chessboard
position. The engine is optimised through the use of lookup tables for piece moves. The board is
represented internally as a set of 64-bit words (as there are 64 squares on a chess board),
structures commonly known in chess programming as bitboards. Many complex calculations
can be performed rapidly through bit operations between bitboards, e.g. rook_moves AND
NOT(occupancy_all)combines a bitboard showing potential rook moves and a bitboard
showing the squares that are not occupied, with the set intersection (boolean AND) showing the
potential rook moves.

#To run

The project currently only consists of unit tests, as game functionality is intended to be added in future. It uses the CMake build system. Out-of-source builds are recommended, create a folder (e.g. /build/), and run:
```
cmake -D GMOCK_DIR=/usr/src/gtest/googlemock -D GTEST_DIR=/usr/src/gtest/googletest  ..
make
```
where ```GMOCK_DIR``` and ```GTEST_DIR``` are directories containing Google Mock and [Google Test](https://github.com/google/googletest). Run the resulting BitChessTests executable and a list of test results should come up. These instructions have only been tested on Linux with GCC; CMake can also generate files for other systems (e.g. Visual Studio) but these have not been tested.

##Overview
BitChess is a simple C++ chess engine. It is intended to be used with [Xboard/Winboard protocol](http://www.gnu.org/software/xboard/engine-intf.html) compatible chess GUIs (with potential UCI support in the future). This doc outlines the implementation and architecture behind the project.

###Goals
[ordered by importance]
####Goals of first version
- Generation of all legal moves in a particular position
- The use of a simpler on-the-fly algorithm for generating moves for sliding pieces (rooks/bishops/queens) based on the [o^(o-2r](https://chessprogramming.wikispaces.com/Subtracting+a+rook+from+a+blocking+piece) trick (with [diagonal](https://chessprogramming.wikispaces.com/Hyperbola+Quintessence) support)
- Selection of a legal move based on a basic evaluation of a position from amount of material
- Full compatibility for use with a XBoard/Winboard GUI (could potentially reuse source from another opensource chess engine)

- Correctness over speed: whilst reasonably optimal data structures shall be used (e.g. hash maps and bitboards) the focus shall be on producing a working engine than a speedy one. Simple calculations such as king and knight move generation will be optimised through the use of lookup tables.
- Time management shall also not be implemented, as searches are effectively 1-ply (meaning only the engine's next move is considered, rather than calculating, e.g. 7 moves ahead)

####Future goals
- Evaluation of a position
- Search for optimal move based on evaluation
- Time management
- The use of [Magic](https://chessprogramming.wikispaces.com/Magic+Bitboards) [Bitboards](http://www.pradu.us/old/Nov27_2008/Buzz/research/magic/Bitboards.pdf) for rapid sliding piece move generation via table lookup
- Optimisation of move generation (e.g. generate moves in an optimal order)
- General optimisation
- UCI compatibility

###Architecture
The central controller is the **Game** class, which communicates with the user through an **XBoard protocol class** and external GUI. The **Game** class keeps track of moves played, and updates the current **Position** with moves received. The **Position** consists of a collection of **Piece** (s) which generate their own pseudo-legal **Move** (s). The **Moves** of each **Piece** on the side to move are combined in a single collection in the **Position** which lists available pseudo-legal moves. An **Evaluator** analyses the consequence of each move, assigning it a score. The **Searcher** searches through the evaluated positions to find the best next move to play, and the **Game** passes this to the interface.
