cmake_minimum_required(VERSION 3.13)

add_executable(
  CTEST_NetworksCApi
  CTEST_NetworksCApi.cpp
)

# the generated cfg and weights files of the tests, every test removes it's
# own ones
target_compile_definitions(
  CTEST_NetworksCApi
  PRIVATE CTEST_NetworksCApi_DATA_DIR=\"${CMAKE_CURRENT_BINARY_DIR}\"
)

target_link_libraries(
  CTEST_NetworksCApi
  DarknetXXNetworksCore
  DarknetXXWeightsFactory
  GTest::gtest_main GTest::gmock
)

include(GoogleTest)

gtest_add_tests(
  TARGET CTEST_NetworksCApi
  TEST_SUFFIX .noArgs
  TEST_LIST noArgsTests
)

set_tests_properties(${noArgsTests} PROPERTIES TIMEOUT 600)
