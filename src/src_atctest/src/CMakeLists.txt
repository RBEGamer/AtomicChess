cmake_minimum_required(VERSION 3.13)
project(src)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

if(NOT CMAKE_BUILD_TYPE)
    message(WARNING "NO CMAKE_BUILD_TYPE SET")
    set(CMAKE_BUILD_TYPE "Release")
endif()

set(VERSION "3.2.3") # FORMAT X.Y.Z single digit
project(atctest)


add_executable(atctest
        main.cpp
        Exception.hpp
        SerialPort.hpp
        SerialPort.cpp
        )

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(atctest PRIVATE DEBUG=1)
    message(STATUS "ADD DEBUG=1")
else()
    target_compile_definitions(atctest PRIVATE DEBUG=0)
endif()

target_compile_definitions(atctest PRIVATE VERSION="${VERSION}")

if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3")
    #set(CMAKE_CXX_FLAGS "-Wall")
    set(CMAKE_CXX_FLAGS_DEBUG "-g")
    message(STATUS "RELEASE BUILD")
endif()


target_link_libraries(atctest
        pthread
        )