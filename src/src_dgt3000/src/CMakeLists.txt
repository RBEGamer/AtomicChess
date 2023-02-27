cmake_minimum_required(VERSION 3.24)
project(atc_dgt3000ext)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

if(NOT CMAKE_BUILD_TYPE)
    message(WARNING "NO CMAKE_BUILD_TYPE SET")
    set(CMAKE_BUILD_TYPE "Release")
endif()

set(VERSION "1.0.0") # FORMAT X.Y.Z single digit

add_executable(atc_dgt3000ext main.cpp dgtpicom.h dgtpicom_dgt3000.h udp_discovery_discovered_peer.hpp udp_discovery_ip_port.cpp udp_discovery_ip_port.hpp udp_discovery_peer.cpp udp_discovery_peer.hpp udp_discovery_peer_parameters.hpp udp_discovery_protocol.cpp udp_discovery_protocol.hpp)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_definitions(atc_dgt3000ext PRIVATE DEBUG=1)
    message(STATUS "ADD DEBUG=1")
else()
    target_compile_definitions(atc_dgt3000ext PRIVATE DEBUG=0)
endif()

target_compile_definitions(atc_dgt3000ext PRIVATE VERSION="${VERSION}")

if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3")
    #set(CMAKE_CXX_FLAGS "-Wall")
    set(CMAKE_CXX_FLAGS_DEBUG "-g")
    message(STATUS "RELEASE BUILD")
endif()

target_link_libraries(atc_dgt3000ext
        pthread
        )
