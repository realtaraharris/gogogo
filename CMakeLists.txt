cmake_minimum_required(VERSION 3.14)
project(GoGameHaiku)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# find_package(Be REQUIRED)

add_executable(go_game src/main.cpp)
target_link_libraries(go_game be)

set_target_properties(go_game PROPERTIES
    LINK_FLAGS "-Wl,-rpath,/system/lib"
)

# install(TARGETS go_game RUNTIME DESTINATION /boot/home/config/non-packaged/bin)
