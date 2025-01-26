cmake_minimum_required(VERSION 3.16)

add_executable(go_engine_test go_engine_test.cpp)
target_link_libraries(go_engine_test PRIVATE gtest_main gtest go_engine)

target_include_directories(go_engine_test PRIVATE
  ${gtest_SOURCE_DIR}/include
  ${gtest_BINARY_DIR}/include
)

add_test(NAME go_engine_test COMMAND go_engine_test)