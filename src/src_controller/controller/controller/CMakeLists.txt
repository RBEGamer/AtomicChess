cmake_minimum_required(VERSION 3.10)
project(atc_controller)
set(PROJECT_NAME atc_controller)

# project version
set(VERSION_MAJOR 1)
set(VERSION_MINOR 0)
set(VERSION_PATCH 0)

# enable c++
enable_language(C CXX)

set (CMAKE_VERBOSE_MAKEFILE 1)
set(CMAKE_CXX_STANDARD 17)
find_package (Threads)


set(SOURCE_FILES SHARED/udp-discovery-cpp-master/udp_discovery_peer.hpp SHARED/udp-discovery-cpp-master/udp_discovery_ip_port.hpp SHARED/udp-discovery-cpp-master/udp_discovery_peer.hpp SHARED/udp-discovery-cpp-master/udp_discovery_peer_parameters.hpp SHARED/udp-discovery-cpp-master/udp_discovery_protocol.hpp SHARED/guicommunicator/guicommunicator.cpp SHARED/json11-master/json11.cpp SHARED/loguru-master/loguru.cpp SHARED/WiringPi-master/wiringPi/piHiPri.c SHARED/WiringPi-master/wiringPi/piThread.c SHARED/WiringPi-master/wiringPi/pseudoPins.c SHARED/WiringPi-master/wiringPi/softPwm.c SHARED/WiringPi-master/wiringPi/softServo.c SHARED/WiringPi-master/wiringPi/softTone.c SHARED/WiringPi-master/wiringPi/wiringPi.c SHARED/WiringPi-master/wiringPi/wiringPiI2C.c SHARED/WiringPi-master/wiringPi/wiringPiSPI.c SHARED/WiringPi-master/wiringPi/wiringSerial.c SHARED/WiringPi-master/wiringPi/wiringShift.c SHARED/udp-discovery-cpp-master/udp_discovery_peer.cpp SHARED/udp-discovery-cpp-master/udp_discovery_ip_port.cpp SHARED/udp-discovery-cpp-master/udp_discovery_peer.cpp SHARED/udp-discovery-cpp-master/udp_discovery_protocol.cpp SerialInterface/SerialInterfaceBase.cpp SerialInterface/SerialInterfaceBase.h SerialInterface/SerialInterfaceSerial.cpp SerialInterface/SerialInterfaceSerial.h SerialInterface/serialib.cpp SerialInterface/serialib.h)

file(GLOB SOURCES *.h *.hpp *.cpp)
add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${SOURCES} HardwareInterface.cpp UserBoardController.cpp UserBoardController.h VoiceRecognitionExtention.cpp VoiceRecognitionExtention.h UDPDiscovery.cpp UDPDiscovery.h DGT3000Interface.cpp DGT3000Interface.h UDPServerDiscovery.cpp UDPServerDiscovery.h)

# MAC COMPILE ON MAC OSX
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
else()
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${Boost_LIBRARIES} rt)
endif()




target_link_libraries(${PROJECT_NAME} ${CMAKE_DL_LIBS})
target_link_libraries (${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(${PROJECT_NAME} ${CMAKE_DL_LIBS})

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "atc_controller")


# SET BUILD RESULT OUTPUT DIRECTORY INTO THE SRC DIRECTORY
set(dir ${CMAKE_CURRENT_SOURCE_DIR})
set(EXECUTABLE_OUTPUT_PATH ${dir} CACHE PATH "Build directory" FORCE)
set(LIBRARY_OUTPUT_PATH ${dir} CACHE PATH "Build directory" FORCE)

# INSTALL TO ATC DIRECOTRY /usr/ATC/atc_controller
set(CMAKE_INSTALL_PREFIX /usr/)
set(INSTALL_DEST ATC)
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${INSTALL_DEST})
