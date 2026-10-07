cmake_minimum_required(VERSION 3.13)

add_executable(
  UTEST_LibraryContext
  UTEST_LibraryContext.cpp
  ${PROJECT_SOURCE_DIR}/src/lib/facade/LibraryContext.cpp
)

target_include_directories(
  UTEST_LibraryContext
  PRIVATE ${PROJECT_SOURCE_DIR}
)

target_link_libraries(
  UTEST_LibraryContext
  GTest::gtest_main
)

include(GoogleTest)

gtest_add_tests(
  TARGET UTEST_LibraryContext
  TEST_SUFFIX .noArgs
  TEST_LIST noArgsTests
)

set_tests_properties(${noArgsTests} PROPERTIES TIMEOUT 600)
