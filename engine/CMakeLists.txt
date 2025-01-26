cmake_minimum_required(VERSION 3.16)
project(GoEngine)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(include)

# Add the main library
add_library(go_engine src/go_engine.cpp)

set_target_properties(go_engine PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

# Testing
# set(CMAKE_CTEST_ARGUMENTS "--verbose") # must be set before enable_testing()
enable_testing()
add_subdirectory(test)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/release-1.11.0.zip
  DOWNLOAD_EXTRACT_TIMESTAMP true
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)
