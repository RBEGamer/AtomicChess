[![pipeline](https://gitlab.com/manzerbredes/uciadapter/badges/main/pipeline.svg)](https://gitlab.com/manzerbredes/uciadapter/-/commits/main)
[![license](https://img.shields.io/badge/License-LGPL_v3-blue.svg)](https://www.gnu.org/licenses/lgpl-3.0)

# uciadapter
*uciadapter* is a C++ library that allows you to communicate with any chess
engines that follows the [UCI Protocol](http://wbec-ridderkerk.nl/html/UCIProtocol.html).
It works on both platforms, Linux and Windows.

# How to use it ?
PGNP can be used as a shared library in your project.
You only need to include `uciadapter.hpp` and linking the .so file to your
executable.

# Example
Somewhere at the beginning of the file:

    #include "uciadapter.hpp"

Example (assuming not catching exceptions):

    uciadapter::UCI u("/path/to/engine");
    u.position("2k2r2/6R1/8/8/8/6Q1/4K3/8 w - - 0 1");
    u.go(uciadapter::Go()); // Launch go with no arguments
    u.SyncAfter(2); // Wait 2s and fetch data from engine
    // Then:
    // u.GetLines(); // To fetch best lines
    // u.Command("<your command>") // Run custom commands
    // u.GetName(); // Engine name
    // u.GetAuthor(); // Fetch engine author

Please look at `UCI.hpp` for more informations on the available API.

# CMake Integration
By using the `add_subdirectory()` directive on this repository, you will be able to use the following cmake calls in your project:

    include_directories(${UCIADAPTER_INCLUDE_DIR})
    target_link_libraries(<YOUR_TARGET> uciadapter)
