cmake_minimum_required(VERSION 3.13)

# The recognitions of the networks of no features computed, which recognize
# every image the way their biases tell.
add_executable(
  CTEST_NetworkRecognizer
  CTEST_NetworkRecognizer.cpp
)

target_compile_definitions(
  CTEST_NetworkRecognizer
  PRIVATE CTEST_NetworkRecognizer_DATA_DIR=\"${CMAKE_CURRENT_BINARY_DIR}\"
)

target_link_libraries(
  CTEST_NetworkRecognizer
  DarknetXXLibraryObjects
  GTest::gtest_main
)

include(GoogleTest)

gtest_add_tests(
  TARGET CTEST_NetworkRecognizer
  TEST_SUFFIX .noArgs
  TEST_LIST noArgsTests
)

set_tests_properties(${noArgsTests} PROPERTIES TIMEOUT 600)
