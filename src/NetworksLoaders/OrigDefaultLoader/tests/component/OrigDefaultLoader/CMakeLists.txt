cmake_minimum_required(VERSION 3.13)

configure_file(
  CTEST_OrigDefaultLoader_provider.h.in
  CTEST_OrigDefaultLoader_provider.h
)

add_executable(
  CTEST_OrigDefaultLoader
  CTEST_OrigDefaultLoader.cpp
  CTEST_OrigDefaultLoader_provider_get_1.cpp
  CTEST_OrigDefaultLoader_provider_get_2.cpp
  NetworkTracer.cpp
)

target_include_directories(
  CTEST_OrigDefaultLoader
  PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(
  CTEST_OrigDefaultLoader
  DarknetXXNetworksCore
  DarknetXXWeightsFactory
  GTest::gtest_main GTest::gmock
)

include(GoogleTest)

gtest_add_tests(
  TARGET CTEST_OrigDefaultLoader
  TEST_SUFFIX .noArgs
  TEST_LIST noArgsTests
)

set_tests_properties(${noArgsTests} PROPERTIES TIMEOUT 600)
