cmake_minimum_required(VERSION 3.13)

# the reference networks and weights files of the loader component test
set(
  CTEST_ORIG_DEFAULT_LOADER_DATA_DIR
  "${CMAKE_SOURCE_DIR}/src/darknet-adaptor/adaptors/NetworksLoaders/OrigDefaultLoader/tests/component/OrigDefaultLoader/data"
)

configure_file(
  CTEST_Dxxwjz1_provider.h.in
  CTEST_Dxxwjz1_provider.h
)

add_executable(
  CTEST_Dxxwjz1
  CTEST_Dxxwjz1.cpp
  CTEST_Dxxwjz1_codec.cpp
  CTEST_Dxxwjz1_parser.cpp
  CTEST_Dxxwjz1_variants.cpp
  Dxxwjz1Checks.cpp
)

target_include_directories(
  CTEST_Dxxwjz1
  PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(
  CTEST_Dxxwjz1
  DarknetXXNetworksCore
  DarknetXXWeightsFactory
  GTest::gtest_main GTest::gmock
)

include(GoogleTest)

# Every TEST_P test runs all the generated variants of its suite in one
# process (the */<suite>.<test>/* ctest names). The dxxwjz1 files are the
# JSON text of every float of the network, so the biggest variants take
# minutes in the sanitizers Debug build.
gtest_add_tests(
  TARGET CTEST_Dxxwjz1
  TEST_SUFFIX .noArgs
  TEST_LIST noArgsTests
)

set_tests_properties(${noArgsTests} PROPERTIES TIMEOUT 1800)
