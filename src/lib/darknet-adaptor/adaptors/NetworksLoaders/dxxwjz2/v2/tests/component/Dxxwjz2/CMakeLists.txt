cmake_minimum_required(VERSION 3.13)

# the reference networks and weights files of the loader component test
set(
  CTEST_ORIG_DEFAULT_LOADER_DATA_DIR
  "${PROJECT_SOURCE_DIR}/src/lib/darknet-adaptor/adaptors/NetworksLoaders/OrigDefaultLoader/tests/component/OrigDefaultLoader/data"
)

configure_file(
  CTEST_Dxxwjz2_provider.h.in
  CTEST_Dxxwjz2_provider.h
)

add_executable(
  CTEST_Dxxwjz2
  CTEST_Dxxwjz2.cpp
  CTEST_Dxxwjz2_parser.cpp
)

target_include_directories(
  CTEST_Dxxwjz2
  PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(
  CTEST_Dxxwjz2
  DarknetXXNetworksCore
  DarknetXXWeightsFactory
  GTest::gtest_main GTest::gmock
)

include(GoogleTest)

gtest_add_tests(
  TARGET CTEST_Dxxwjz2
  TEST_SUFFIX .noArgs
  TEST_LIST noArgsTests
)

set_tests_properties(${noArgsTests} PROPERTIES TIMEOUT 1800)
