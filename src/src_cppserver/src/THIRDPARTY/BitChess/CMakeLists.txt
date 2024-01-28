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

include_directories(
  src/
  test/
 )

find_package(Threads REQUIRED)





# add source files
#add_executable(
#  src/position/pieces/pawn.cpp
#  src/position/pieces/pieces.cpp
#  src/util/bitboard.cpp
#  #src/main.cpp # disabled for tests
#  src/xboard.cpp
#)




if(THREADS_HAVE_PTHREAD_ARG)
  set_property(TARGET BitChess PROPERTY COMPILE_OPTIONS "-pthread")
  set_property(TARGET BitChess PROPERTY INTERFACE_COMPILE_OPTIONS "-pthread")
endif()

