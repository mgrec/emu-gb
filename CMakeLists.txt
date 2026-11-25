cmake_minimum_required(VERSION 3.25)
project(emu-gb LANGUAGES CXX)

# Standard C++20 only (no compiler-specific extensions)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Debug by default when no build type is given
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Compiler warnings
if(MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -Wpedantic)
    # Debug: report out-of-bounds accesses and undefined behaviour at runtime
    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
        add_link_options(-fsanitize=address,undefined)
    endif()
endif()

# --- Emulator ---------------------------------------------------------------
# Add each new .cpp file to this list.
add_executable(emu-gb
    src/main.cpp
    src/cartridge.cpp
)

# --- Unit tests (doctest) ---------------------------------------------------
include(FetchContent)
FetchContent_Declare(doctest
    GIT_REPOSITORY https://github.com/doctest/doctest.git
    GIT_TAG        v2.5.3
    GIT_SHALLOW    TRUE
    SYSTEM
)
FetchContent_MakeAvailable(doctest)

# Add each new test file to this list.
add_executable(emu-gb-tests
    tests/test_main.cpp
)
target_link_libraries(emu-gb-tests PRIVATE doctest::doctest)

enable_testing()
add_test(NAME unit-tests COMMAND emu-gb-tests)
