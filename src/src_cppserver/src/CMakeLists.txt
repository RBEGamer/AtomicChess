cmake_minimum_required(VERSION 3.10)
project(atc_server)
set(PROJECT_NAME atc_server)

# project version
set(VERSION_MAJOR 1)
set(VERSION_MINOR 0)
set(VERSION_PATCH 0)

# enable c++
enable_language(C CXX)

set (CMAKE_VERBOSE_MAKEFILE 1)
set(CMAKE_CXX_STANDARD 17)
find_package (Threads)


set(SOURCE_FILES main.cpp SHARED/json11-master/json11.cpp SHARED/loguru-master/loguru.cpp)

file(GLOB SOURCES *.h *.hpp *.cpp)
add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${SOURCES})

# MAC COMPILE ON MAC OSX
if (${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
else()
    TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${Boost_LIBRARIES} rt)
endif()




target_link_libraries(${PROJECT_NAME} ${CMAKE_DL_LIBS})
target_link_libraries (${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(${PROJECT_NAME} ${CMAKE_DL_LIBS})

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "atc_server")


# SET BUILD RESULT OUTPUT DIRECTORY INTO THE SRC DIRECTORY
set(dir ${CMAKE_CURRENT_SOURCE_DIR})
set(EXECUTABLE_OUTPUT_PATH ${dir} CACHE PATH "Build directory" FORCE)
set(LIBRARY_OUTPUT_PATH ${dir} CACHE PATH "Build directory" FORCE)

# INSTALL TO ATC DIRECOTRY /usr/ATC/atc_controller
set(CMAKE_INSTALL_PREFIX /usr/)
set(INSTALL_DEST ATC)
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${INSTALL_DEST})
