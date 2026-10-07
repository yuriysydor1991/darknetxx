cmake_minimum_required(VERSION 3.13)

add_executable(
  UTEST_TrainingProgress
  UTEST_TrainingProgress.cpp
  ${PROJECT_SOURCE_DIR}/src/lib/facade/TrainingProgress.cpp
)

target_include_directories(
  UTEST_TrainingProgress
  PRIVATE ${PROJECT_SOURCE_DIR}
)

target_link_libraries(
  UTEST_TrainingProgress
  GTest::gtest_main
)

include(GoogleTest)

gtest_add_tests(
  TARGET UTEST_TrainingProgress
  TEST_SUFFIX .noArgs
  TEST_LIST noArgsTests
)

set_tests_properties(${noArgsTests} PROPERTIES TIMEOUT 600)
