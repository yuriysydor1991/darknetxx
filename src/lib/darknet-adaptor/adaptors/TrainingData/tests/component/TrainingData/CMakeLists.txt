cmake_minimum_required(VERSION 3.13)

add_executable(
  CTEST_TrainingData
  CTEST_TrainingData.cpp
)

target_link_libraries(
  CTEST_TrainingData
  DarknetXXNetworksCore
  GTest::gtest_main
)

include(GoogleTest)

gtest_add_tests(
  TARGET CTEST_TrainingData
  TEST_SUFFIX .noArgs
  TEST_LIST noArgsTests
)
