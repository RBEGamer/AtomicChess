cmake_minimum_required(VERSION 3.24)
project(dgt3000ext)

set(CMAKE_CXX_STANDARD 11)

add_executable(dgt3000ext main.cpp dgtpicom.h dgtpicom_dgt3000.h udp_discovery_discovered_peer.hpp udp_discovery_ip_port.cpp udp_discovery_ip_port.hpp udp_discovery_peer.cpp udp_discovery_peer.hpp udp_discovery_peer_parameters.hpp udp_discovery_protocol.cpp udp_discovery_protocol.hpp)
