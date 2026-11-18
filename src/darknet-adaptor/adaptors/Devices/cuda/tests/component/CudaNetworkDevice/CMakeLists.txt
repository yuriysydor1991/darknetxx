cmake_minimum_required(VERSION 3.13)

add_executable(
  CTEST_CudaNetworkDevice
  CTEST_CudaNetworkDevice.cpp
)

target_link_libraries(
  CTEST_CudaNetworkDevice
  DarknetXXNetworksCore
  DarknetXXWeightsFactory
  GTest::gtest_main
)

include(GoogleTest)

gtest_add_tests(
  TARGET CTEST_CudaNetworkDevice
  TEST_SUFFIX .noArgs
  TEST_LIST noArgsTests
)

set_tests_properties(${noArgsTests} PROPERTIES TIMEOUT 600)
