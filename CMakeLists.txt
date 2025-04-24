cmake_minimum_required(VERSION 3.24)

# ——————————————————————————
# Project definition
# ——————————————————————————
project(StdVectorArray VERSION 1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD     23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# ——————————————————————————
# User‑configurable options
# ——————————————————————————
option(WARNINGS_AS_ERRORS  "Treat all compiler warnings as errors"       OFF)
option(ENABLE_PVS_STUDIO   "Run PVS‑Studio static analysis checks"       OFF)
option(ENABLE_UBSAN        "Enable UndefinedBehaviorSanitizer"          ON)
option(ENABLE_ASAN         "Enable AddressSanitizer"                    ON)
option(ENABLE_TSAN         "Enable ThreadSanitizer"                     OFF)
option(ENABLE_MSAN         "Enable MemorySanitizer"                     ON)
option(ENABLE_CLANG_TIDY   "Enable clang‑tidy static analysis checks"  ON)

# If you turned on clang-tidy, point CMake at it here:
if (ENABLE_CLANG_TIDY)
    find_program(CLANG_TIDY_EXE NAMES clang-tidy)
    if (CLANG_TIDY_EXE)
        set(CMAKE_CXX_CLANG_TIDY
            "${CLANG_TIDY_EXE};-checks=*,clang-analyzer-*,-clang-analyzer-cplusplus*")
    endif()
endif()

# ——————————————————————————
# Warnings and sanitizers
# ——————————————————————————
include(cmake/CompilerWarnings.cmake)   # your warning flags & WARNINGS_AS_ERRORS logic
# (cmake/main-config.cmake will pick up ENABLE_*SAN and ALL_TARGETS)

# ——————————————————————————
# Main executable target
# ——————————————————————————
add_executable(${PROJECT_NAME}
    main.cpp
    options_parser/options_parser.cpp
    options_parser/options_parser.h
)

target_include_directories(${PROJECT_NAME} PRIVATE
    options_parser
)

# ——————————————————————————
# Dependencies
# ——————————————————————————
find_package(Boost 1.71 REQUIRED COMPONENTS program_options system)
target_link_libraries(${PROJECT_NAME} PRIVATE
    Boost::program_options
    Boost::system
)

# ——————————————————————————
# GoogleTest
# ——————————————————————————
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        release-1.12.1
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

# Test executable
# dirname(${CMAKE_CURRENT_SOURCE_DIR}/tests/tests.cpp TEST_SRC_DIR)
add_executable(${PROJECT_NAME}_tests
    tests/tests.cpp
)

target_include_directories(${PROJECT_NAME}_tests PRIVATE
    ${PROJECT_SOURCE_DIR}
)
target_link_libraries(${PROJECT_NAME}_tests PRIVATE
    GTest::gtest_main
)

add_test(NAME my_array_tests COMMAND ${PROJECT_NAME}_tests)

# ——————————————————————————
# Installation
# ——————————————————————————
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)

# ——————————————————————————
# Final includes
# ——————————————————————————
set(ALL_TARGETS ${PROJECT_NAME} ${PROJECT_NAME}_tests)
include(cmake/main-config.cmake)
