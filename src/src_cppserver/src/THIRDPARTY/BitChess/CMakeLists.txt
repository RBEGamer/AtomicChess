cmake_minimum_required (VERSION 2.6)
project (BitChess C CXX)



# C++11 support

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
    message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

# Version
set (BitChess_VERSION_MAJOR 1)
set (BitChess_VERSION_MINOR 0)

set(THC_CHESS_INCLUDE ${PROJECT_SOURCE_DIR}/BitChess PARENT_SCOPE)

include_directories(BitChess/)

find_package(Threads REQUIRED)


# add source files
add_executable(BitChess
        ${PROJECT_SOURCE_DIR}/BitChess/position/pieces/pawn.cpp
        ${PROJECT_SOURCE_DIR}/BitChess/position/pieces/pieces.cpp
        ${PROJECT_SOURCE_DIR}/BitChess/util/bitboard.cpp
)



if(THREADS_HAVE_PTHREAD_ARG)
    set_property(TARGET BitChess PROPERTY COMPILE_OPTIONS "-pthread")
    set_property(TARGET BitChess PROPERTY INTERFACE_COMPILE_OPTIONS "-pthread")
endif()

file(GLOB_RECURSE THC_BITCHESS_SRCS ${PROJECT_SOURCE_DIR}/BitChess/*.cpp ${PROJECT_SOURCE_DIR}/BitChess/*.h ${PROJECT_SOURCE_DIR}/BitChess/**.hpp)

add_library(BitChess_shared SHARED ${THC_BITCHESS_SRCS})
add_library(BitChess_static STATIC ${THC_BITCHESS_SRCS})
