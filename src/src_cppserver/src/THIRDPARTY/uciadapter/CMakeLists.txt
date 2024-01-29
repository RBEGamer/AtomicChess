cmake_minimum_required(VERSION 3.10)
project(uciadapter)

# Configure Process
SET(process src/ProcessLinux.cpp)
SET(COMPILE_PLATFORM UNIX)
if(WIN32)
    SET(process src/ProcessWindows.cpp)
    SET(COMPILE_PLATFORM WIN32)
    SET(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

add_library(uciadapter STATIC src/UCI.cpp ${process})

# Includes
set(UCIADAPTER_INCLUDE_DIR ${CMAKE_CURRENT_BINARY_DIR}/includes) # For conveniance
set(UCIADAPTER_INCLUDE_DIR ${UCIADAPTER_INCLUDE_DIR} PARENT_SCOPE) # To be used by other projects with add_subdirectory()
file(MAKE_DIRECTORY ${UCIADAPTER_INCLUDE_DIR})
configure_file(src/UCI.hpp.in ${UCIADAPTER_INCLUDE_DIR}/UCI.hpp)
configure_file(src/Process.hpp ${UCIADAPTER_INCLUDE_DIR} COPYONLY)
configure_file(src/ProcessLinux.hpp ${UCIADAPTER_INCLUDE_DIR} COPYONLY)
configure_file(src/ProcessWindows.hpp ${UCIADAPTER_INCLUDE_DIR} COPYONLY)
target_include_directories(uciadapter PUBLIC ${UCIADAPTER_INCLUDE_DIR})

# Tests
# Unit tests
set(COMPILE_TESTS OFF CACHE BOOL "Should unit tests be compiled")
if(COMPILE_TESTS)
    enable_testing()
    add_subdirectory("tests/")
endif()
