cmake_minimum_required(VERSION 3.14)

project(ChessNGin
        VERSION 0.1
        LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

if(CMAKE_BUILD_TYPE MATCHES Debug)
    if(MSVC)
        #/WX
        add_compile_options(/W4)
    else()
        # Flags to be added once refactor is done
        #-Werror
        add_compile_options(-Wno-unused-parameter)

        # Code Coverage - download and append compiler flags
        file(DOWNLOAD
             https://raw.githubusercontent.com/bilke/cmake-modules/master/CodeCoverage.cmake
             ${CMAKE_SOURCE_DIR}/cmake/CodeCoverage.cmake
             SHOW_PROGRESS
        )
        include(CodeCoverage)
        append_coverage_compiler_flags()
    endif()
endif()

if(CMAKE_PROJECT_NAME STREQUAL ChessNGin AND BUILD_TESTING)
    message(STATUS "Testing enabled")
    enable_testing()
    add_subdirectory(tests)
endif()

add_subdirectory(src)