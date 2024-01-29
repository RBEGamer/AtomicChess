cmake_minimum_required(VERSION 3.0)

project(udp-discovery-cpp)

option(BUILD_EXAMPLE "Build example application." OFF)
option(BUILD_TOOL "Build udp-discovery-tool application." OFF)

set(LIB_SOURCES
	udp_discovery_ip_port.cpp
	udp_discovery_peer.cpp
	udp_discovery_protocol.cpp)
set(LIB_HEADERS
	udp_discovery_ip_port.hpp
	udp_discovery_peer.hpp
	udp_discovery_protocol.hpp)

add_library(udp-discovery STATIC ${LIB_SOURCES} ${LIB_HEADERS})


	if(APPLE)
	elseif(UNIX)
		set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pthread")
	endif()



	set(DISCOVERY_EXAMPLE_SOURCES atc_discovery_helper.cpp)
	set(DISCOVERY_EXAMPLE_LIBS udp-discovery)

	if(APPLE)
	elseif(UNIX)
		set(DISCOVERY_EXAMPLE_LIBS ${DISCOVERY_EXAMPLE_LIBS} rt)
	endif()

	if(WIN32)
		set(DISCOVERY_EXAMPLE_LIBS ${DISCOVERY_EXAMPLE_LIBS} Ws2_32)
	endif(WIN32)

	add_executable(atc_discovery_helper ${DISCOVERY_EXAMPLE_SOURCES})
	target_link_libraries(atc_discovery_helper ${DISCOVERY_EXAMPLE_LIBS})


