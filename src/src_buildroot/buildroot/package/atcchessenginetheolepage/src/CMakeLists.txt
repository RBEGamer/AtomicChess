# SETUP
cmake_minimum_required(VERSION 3.12)
project(Chess
        DESCRIPTION "Chess Engine & Chess AI using standard protocols"
        VERSION 0.1
        HOMEPAGE_URL "https://gitlab.cri.epita.fr/theo.lepage/chess"
        LANGUAGES CXX)

#[[
----- USAGE -----
1. Create and go to a directory of build (preference : debug or release) :
    - `mkdir build_debug` / `mkdir build_release`
    - `cd build_debug` / `cd build_release`
    (Use `build_debug` folder to use a Debug BUILD_TYPE)

2. Generate cmake files in build directory with the good options :
    - `cmake .. [OPTIONS]`
        - Set Compiler  : -DCMAKE_C_COMPILER=/usr/bin/g++
        - Debug build   : -DCMAKE_BUILD_TYPE=Debug
        - Release build : -DCMAKE_BUILD_TYPE=Release

3. Use make as usual in the wanted build_directory: (use -j8 to go swifty)
    - `make`
    - `make all`
    - `make clean`
    - `make check`
    - `make -B`
    - verbose option : `VERBOSE=1 make`

4. verbose tests
    - ctest --verbose
]]

# OPTIMISATION FLAGS
set(OPT_FLAGS
    -Ofast # Standard compliance is no more
    -march=native # Allowed to use CPU special instruction set
    -flto # Link time optimizer
    -funroll-loops # Self explanatory & enables other opti (rename register, fweb)

    # All below accelate not much & are dangerous, use only if all test pass + works on PIE
    -fmerge-all-constants # Self explanatory but dangerous and not bug speed up
    -fgcse-sm # subexpression elimination
    -fgcse-las # Subexpression elimination
    -fgcse-after-reload # Clean up redundant spilling
    -fdelete-null-pointer-checks # Self expl
    -fdevirtualize-speculatively # Opti call to virtual fonc
    -fdevirtualize-at-ltrans # Opti call to virtual fonc
    -fno-lifetime-dse # Dangerous if we use field of object after object destruction
    -ftree-loop-im # Loop invariant motion on trees

    #FIXME
    )
string(REPLACE ";" " " OPT_FLAGS "${OPT_FLAGS}")

# SOURCES
set(MAIN_ENGINE
    src/main.cc)
set(SRC_ENGINE
    src/chess_engine/ai/ai-launcher.cc
    src/chess_engine/ai/ai-mini.cc
    src/chess_engine/ai/evaluation.cc
    src/chess_engine/ai/uci.cc
    src/chess_engine/board/move-initialization.cc
    src/chess_engine/board/move-generation.cc
    src/chess_engine/board/chessboard.cc
    src/chess_engine/board/board.cc
    src/parsing/option_parser/option-parser.cc
    src/parsing/perft_parser/perft-parser.cc
    src/parsing/pgn_parser/pgn-exception.cc
    src/parsing/pgn_parser/pgn-move.cc
    src/parsing/pgn_parser/pgn-parser.cc
    src/listener/listener-manager.cc
    )
include_directories(src
                    #FIXME
                    )

# TESTS
set(SRC_TESTS
    tests/unit_tests/example_test.cc
    tests/unit_tests/example2_test.cc
    tests/unit_tests/perft_parser_test.cc
    tests/unit_tests/chessboard_test.cc
    tests/unit_tests/fenrank_test.cc
    tests/unit_tests/fenobject_test.cc
    tests/unit_tests/possible_move_test.cc
    tests/unit_tests/check_test.cc
    tests/unit_tests/game_test.cc
    tests/unit_tests/ai_search_test.cc
    tests/unit_tests/ai_search2_test.cc
    tests/unit_tests/evaluate_test.cc
    tests/unit_tests/uci_communication_test.cc
    tests/unit_tests/board_test.cc
    tests/unit_tests/move_generation_test.cc
    #FIXME
    )

# MAKE CHECK_PGN TARGET
add_custom_target(check_pgn
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
    COMMAND python3 ${CMAKE_SOURCE_DIR}/tests/pgn-testsuite.py
             -v ${CMAKE_SOURCE_DIR}/chessengine
             ${CMAKE_SOURCE_DIR}/tests/libbasic-output.so
             ${CMAKE_SOURCE_DIR}/tests/given_pgn
)

# MAKE CHECK_PERFT TARGET
add_custom_target(check_perft
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
    COMMAND python3 ${CMAKE_SOURCE_DIR}/tests/perft-testsuite.py
             -v ${CMAKE_SOURCE_DIR}/chessengine
             ${CMAKE_SOURCE_DIR}/tests/given_perft;
)

# MAKE CHECK_UNIT TARGET
add_custom_target(check_unit
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
    COMMAND ${CMAKE_CTEST_COMMAND} --verbose
)

# MAKE CHECK TARGET
add_custom_target(check
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
    COMMAND python3 ${CMAKE_SOURCE_DIR}/tests/pgn-testsuite.py
             -v ${CMAKE_SOURCE_DIR}/chessengine
             ${CMAKE_SOURCE_DIR}/tests/libbasic-output.so
             ${CMAKE_SOURCE_DIR}/tests/given_pgn
    COMMAND python3 ${CMAKE_SOURCE_DIR}/tests/perft-testsuite.py
             -v ${CMAKE_SOURCE_DIR}/chessengine
             ${CMAKE_SOURCE_DIR}/tests/given_perft;
)
add_dependencies(check check_unit)

# BUILD OPTIONS
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wextra -pedantic")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${OPT_FLAGS}")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g3 -fsanitize=address")
string(REPLACE "-O3" "" CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE}")


# BUILD_TYPE (if not defined by the user) + lichess
if (NOT CMAKE_BUILD_TYPE)
    string(REGEX REPLACE ".*/build_" "" BUILD_NAME ${CMAKE_BINARY_DIR})
    # Default BUILD_TYPE is Release
    set(CMAKE_BUILD_TYPE Release)
    # If folder is named "build_debug" BUILD_TYPE is set to: Debug
    if(${BUILD_NAME} STREQUAL "Debug" OR ${BUILD_NAME} STREQUAL "debug")
        set(CMAKE_BUILD_TYPE Debug)
    endif()
    if(${BUILD_NAME} STREQUAL "lichess" OR ${BUILD_NAME} STREQUAL "Lichess")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DLICHESS")
    endif()
    if(${BUILD_NAME} STREQUAL "gprof")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pg")
    endif()
    message(STATUS "Build type : ${CMAKE_BUILD_TYPE}")
endif()

# LIBRARIES
if (CMAKE_BUILD_TYPE STREQUAL "Release")
    set(Boost_USE_STATIC_LIBS ON)
endif()
find_package(Boost REQUIRED COMPONENTS system program_options)
set(LIBRARIES Boost::system Boost::program_options ${CMAKE_DL_LIBS})

# LIBRARIE OF SRC_FILES
add_library(SRC_ENGINE_OBJ ${SRC_ENGINE})

# CHESSENGINE
add_executable(chessengine)
set_target_properties(chessengine PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}) # binary destination
target_sources(chessengine PRIVATE ${MAIN_ENGINE})
target_link_libraries(chessengine PRIVATE SRC_ENGINE_OBJ ${LIBRARIES})
add_dependencies(check_perft chessengine)
add_dependencies(check_pgn chessengine)
add_dependencies(check_unit chessengine)

# STATIC TARGET
if (CMAKE_BUILD_TYPE STREQUAL "Release")
    add_executable(chessengine-static)
    set_target_properties(chessengine-static PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}) # binary destination
    target_sources(chessengine-static PRIVATE ${MAIN_ENGINE})
    target_link_libraries(chessengine-static PRIVATE SRC_ENGINE_OBJ ${LIBRARIES} -static)
    add_dependencies(check_perft chessengine-static)
    add_dependencies(check_pgn chessengine-static)
    add_dependencies(check_unit chessengine-static)
endif()

