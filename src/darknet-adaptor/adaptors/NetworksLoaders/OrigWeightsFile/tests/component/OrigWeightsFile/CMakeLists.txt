cmake_minimum_required(VERSION 3.13)

# the reference networks and weights files of the loader component test
set(
  CTEST_ORIG_DEFAULT_LOADER_DATA_DIR
  "${CMAKE_CURRENT_SOURCE_DIR}/../../../../OrigDefaultLoader/tests/component/OrigDefaultLoader/data"
)

configure_file(
  CTEST_OrigWeightsFile_provider.h.in
  CTEST_OrigWeightsFile_provider.h
)

add_executable(
  CTEST_OrigWeightsFile
  CTEST_OrigWeightsFile.cpp
  CTEST_OrigWeightsFile_variants.cpp
  DarknetNetwork.cpp
  WeightsChecks.cpp
)

target_include_directories(
  CTEST_OrigWeightsFile
  PRIVATE
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(
  CTEST_OrigWeightsFile
  DarknetXXNetworksCore
  DarknetXXWeightsFactory
  GTest::gtest_main GTest::gmock
)

include(GoogleTest)

# Every TEST_P test runs all the generated variants of it's suite in one
# process (the */<suite>.<test>/* ctest names), the biggest ones write the
# weights files of the hundreds of megabytes. The slowest of them takes about
# 80 seconds in the sanitizers Debug build.
gtest_add_tests(
  TARGET CTEST_OrigWeightsFile
  TEST_SUFFIX .noArgs
  TEST_LIST noArgsTests
)

set_tests_properties(${noArgsTests} PROPERTIES TIMEOUT 600)
