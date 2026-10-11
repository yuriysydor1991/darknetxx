cmake_minimum_required(VERSION 3.13)

add_executable(
  CTEST_ALayerAdaptor
  CTEST_ALayerAdaptor.cpp
)

target_link_libraries(
  CTEST_ALayerAdaptor
  DarknetXXNetworksCore
  GTest::gtest_main GTest::gmock
)

include(GoogleTest)

gtest_add_tests(
  TARGET CTEST_ALayerAdaptor
  TEST_SUFFIX .noArgs
  TEST_LIST noArgsTests
)

set_tests_properties(${noArgsTests} PROPERTIES TIMEOUT 60)
