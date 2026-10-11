cmake_minimum_required(VERSION 3.13)

add_executable(
  CTEST_ANetworkAdaptor
  CTEST_ANetworkAdaptor.cpp
)

# the generated cfg and weights files of the tests, every test removes it's
# own ones
target_compile_definitions(
  CTEST_ANetworkAdaptor
  PRIVATE CTEST_ANetworkAdaptor_DATA_DIR=\"${CMAKE_CURRENT_BINARY_DIR}\"
)

target_link_libraries(
  CTEST_ANetworkAdaptor
  DarknetXXNetworksCore
  DarknetXXWeightsFactory
  GTest::gtest_main GTest::gmock
)

include(GoogleTest)

gtest_add_tests(
  TARGET CTEST_ANetworkAdaptor
  TEST_SUFFIX .noArgs
  TEST_LIST noArgsTests
)

set_tests_properties(${noArgsTests} PROPERTIES TIMEOUT 600)
