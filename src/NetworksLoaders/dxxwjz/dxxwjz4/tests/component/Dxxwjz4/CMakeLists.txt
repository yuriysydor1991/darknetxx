cmake_minimum_required(VERSION 3.13)

# the reference networks and weights files of the loader component test
set(
  CTEST_ORIG_DEFAULT_LOADER_DATA_DIR
  "${PROJECT_SOURCE_DIR}/src/NetworksLoaders/OrigDefaultLoader/tests/component/OrigDefaultLoader/data"
)

add_executable(
  CTEST_Dxxwjz4
  CTEST_Dxxwjz4.cpp
)

target_compile_definitions(
  CTEST_Dxxwjz4
  PRIVATE CTEST_Dxxwjz4_DATA_DIR=\"${CMAKE_CURRENT_BINARY_DIR}\"
  PRIVATE CTEST_Dxxwjz4_CFG=\"${CTEST_ORIG_DEFAULT_LOADER_DATA_DIR}/1.cfg\"
  PRIVATE CTEST_Dxxwjz4_WEIGHTS=\"${CTEST_ORIG_DEFAULT_LOADER_DATA_DIR}/1.weights\"
)

target_link_libraries(
  CTEST_Dxxwjz4
  DarknetXXNetworksCore
  GTest::gtest_main
)

include(GoogleTest)

gtest_add_tests(
  TARGET CTEST_Dxxwjz4
  TEST_SUFFIX .noArgs
  TEST_LIST noArgsTests
)

set_tests_properties(${noArgsTests} PROPERTIES TIMEOUT 600)
