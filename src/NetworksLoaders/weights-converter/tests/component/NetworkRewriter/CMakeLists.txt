cmake_minimum_required(VERSION 3.13)

add_executable(
  CTEST_NetworkRewriter
  CTEST_NetworkRewriter.cpp
)

# the generated cfg, weights and project files of the tests, every test
# removes its own ones
target_compile_definitions(
  CTEST_NetworkRewriter
  PRIVATE
    CTEST_NetworkRewriter_DATA_DIR=\"${CMAKE_CURRENT_BINARY_DIR}/generated\"
)

target_link_libraries(
  CTEST_NetworkRewriter
  DarknetXXNetworksCore
  DarknetXXWeightsFactory
  GTest::gtest_main GTest::gmock
)

include(GoogleTest)

gtest_add_tests(
  TARGET CTEST_NetworkRewriter
  TEST_SUFFIX .noArgs
  TEST_LIST noArgsTests
)

set_tests_properties(${noArgsTests} PROPERTIES TIMEOUT 600)
