# Download CPM.cmake
file(
    DOWNLOAD
    https://github.com/cpm-cmake/CPM.cmake/releases/download/v0.38.3/CPM.cmake
    ${CMAKE_SOURCE_DIR}/cmake/CPM.cmake
    EXPECTED_HASH SHA256=cc155ce02e7945e7b8967ddfaff0b050e958a723ef7aad3766d368940cb15494
    SHOW_PROGRESS
)

include(CPM)
CPMAddPackage(
    NAME Catch2
    GIT_REPOSITORY "https://github.com/catchorg/Catch2.git"
    GIT_TAG "v3.5.0"
)

list(APPEND CMAKE_MODULE_PATH "${Catch2_SOURCE_DIR}/extras")

# These tests can use the Catch2-provided main
add_executable(tests test.cpp)
target_include_directories(tests PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain nginlib)
add_dependencies(tests nginlib)

include(CTest)
include(Catch)
catch_discover_tests(tests)

if( NOT MSVC AND CMAKE_BUILD_TYPE MATCHES Debug)
    setup_target_for_coverage_lcov(NAME coverage
                                    EXECUTABLE tests
                                    EXCLUDE "/usr/*" "cmake/catch2/*"
                                    DEPENDENCIES tests nginlib)
endif()