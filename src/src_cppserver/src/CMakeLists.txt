cmake_minimum_required(VERSION 3.16)
project(atc_server)
set(PROJECT_NAME atc_server)

# project version
set(VERSION_MAJOR 1)
set(VERSION_MINOR 0)
set(VERSION_PATCH 0)

set(VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")

# enable c++
enable_language(C CXX)

set (CMAKE_VERBOSE_MAKEFILE 1)
set(CMAKE_CXX_STANDARD 17)
FIND_PACKAGE (Threads)
FIND_PACKAGE(SQLite3 REQUIRED)

add_subdirectory("${PROJECT_SOURCE_DIR}/THIRDPARTY/fmt")





set(SOURCE_FILES SHARED/json11-master/json11.cpp SHARED/loguru-master/loguru.cpp
        db_adapter.cpp
        db_adapter.h
        game_state.cpp
        game_state.h)



add_subdirectory("${PROJECT_SOURCE_DIR}/THIRDPARTY/thc-chess-library")
include_directories(${THC_CHESS_INCLUDE})

add_subdirectory("${PROJECT_SOURCE_DIR}/THIRDPARTY/uciadapter")
include_directories(${UCIADAPTER_INCLUDE_DIR})



file(GLOB SOURCES *.h *.hpp *.cpp)

add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${SOURCES})


TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${SQLite3_LIBRARIES} fmt::fmt thc_chess_static uciadapter)

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

# INSTALL TO ATC DIRECOTRY /usr/ATC/atc_server
set(CMAKE_INSTALL_PREFIX /usr/)
set(INSTALL_DEST ATC)
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${INSTALL_DEST})
