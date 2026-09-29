cmake_minimum_required(VERSION 3.13)

add_executable(
  CTEST_CfgSection
  CTEST_CfgSection.cpp
)

# the cfg and the log files of the tests, every test removes it's own ones
target_compile_definitions(
  CTEST_CfgSection
  PRIVATE CTEST_CfgSection_DATA_DIR=\"${CMAKE_CURRENT_BINARY_DIR}\"
)

target_link_libraries(
  CTEST_CfgSection
  DarknetXXNetworksCore
  GTest::gtest_main GTest::gmock
)

include(GoogleTest)

gtest_add_tests(
  TARGET CTEST_CfgSection
  TEST_SUFFIX .noArgs
  TEST_LIST noArgsTests
)

set_tests_properties(${noArgsTests} PROPERTIES TIMEOUT 60)
