set(SOURCES main.cpp piece.cpp)
add_library(nginlib STATIC
    piece.hpp
    player.hpp
    board.hpp
    piece.cpp
    player.cpp
    board.cpp
)

add_executable(NGin ${SOURCES})

if(MSVC)
    set(ENABLED_WARNINGS /Wall)
else()
    set(ENABLED_WARNING -Wall -Wextra -Wshadow -Wnon-virtual-dtor -pedantic -Wold-style-cast -Wcast-align
    -Woverloaded-virtual -Wconversion -Wsign-conversion -Wmisleading-indentation -Wduplicated-cond
    -Wduplicated-branches -Wlogical-op -Wnull-dereference -Wuseless-cast -Wdouble-promotion
    -Wformat=2)
endif()

target_compile_options(nginlib PRIVATE ${ENABLED_WARNING})
target_compile_options(NGin PRIVATE ${ENABLED_WARNING})