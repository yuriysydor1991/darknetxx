cmake_minimum_required(VERSION 3.13)

add_executable(
  CTEST_TrainingChart
  CTEST_TrainingChart.cpp
)

# the generated charts of the tests, every test removes its own ones
target_compile_definitions(
  CTEST_TrainingChart
  PRIVATE CTEST_TrainingChart_DATA_DIR=\"${CMAKE_CURRENT_BINARY_DIR}/generated\"
)

target_link_libraries(
  CTEST_TrainingChart
  DarknetXXNetworksCore
  GTest::gtest_main
)

include(GoogleTest)

gtest_add_tests(
  TARGET CTEST_TrainingChart
  TEST_SUFFIX .noArgs
  TEST_LIST noArgsTests
)

set_tests_properties(${noArgsTests} PROPERTIES TIMEOUT 600)
