cmake_minimum_required(VERSION 3.13)

add_executable(
  CTEST_WeightsFileFormats
  CTEST_WeightsFileFormats.cpp
)

target_link_libraries(
  CTEST_WeightsFileFormats
  DarknetXXNetworksCore
  GTest::gtest_main
)

include(GoogleTest)

gtest_add_tests(
  TARGET CTEST_WeightsFileFormats
  TEST_SUFFIX .noArgs
  TEST_LIST noArgsTests
)

set_tests_properties(${noArgsTests} PROPERTIES TIMEOUT 600)
