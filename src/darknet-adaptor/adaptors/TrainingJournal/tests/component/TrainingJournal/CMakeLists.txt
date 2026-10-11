cmake_minimum_required(VERSION 3.13)

add_executable(
  CTEST_TrainingJournal
  CTEST_TrainingJournal.cpp
)

# the generated files of the tests, every test removes its own ones
target_compile_definitions(
  CTEST_TrainingJournal
  PRIVATE CTEST_TrainingJournal_DATA_DIR=\"${CMAKE_CURRENT_BINARY_DIR}/generated\"
)

target_link_libraries(
  CTEST_TrainingJournal
  DarknetXXNetworksCore
  DarknetXXWeightsFactory
  GTest::gtest_main
)

include(GoogleTest)

gtest_add_tests(
  TARGET CTEST_TrainingJournal
  TEST_SUFFIX .noArgs
  TEST_LIST noArgsTests
)

set_tests_properties(${noArgsTests} PROPERTIES TIMEOUT 600)
