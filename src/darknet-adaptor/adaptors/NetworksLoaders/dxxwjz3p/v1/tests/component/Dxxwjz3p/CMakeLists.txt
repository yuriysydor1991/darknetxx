cmake_minimum_required(VERSION 3.13)

# the reference networks and weights files of the loader component test
set(
  CTEST_ORIG_DEFAULT_LOADER_DATA_DIR
  "${PROJECT_SOURCE_DIR}/src/darknet-adaptor/adaptors/NetworksLoaders/OrigDefaultLoader/tests/component/OrigDefaultLoader/data"
)

add_executable(
  CTEST_Dxxwjz3p
  CTEST_Dxxwjz3p.cpp
)

target_compile_definitions(
  CTEST_Dxxwjz3p
  PRIVATE CTEST_Dxxwjz3p_DATA_DIR=\"${CMAKE_CURRENT_BINARY_DIR}\"
  PRIVATE CTEST_Dxxwjz3p_CFG=\"${CTEST_ORIG_DEFAULT_LOADER_DATA_DIR}/1.cfg\"
  PRIVATE CTEST_Dxxwjz3p_WEIGHTS=\"${CTEST_ORIG_DEFAULT_LOADER_DATA_DIR}/1.weights\"
)

target_link_libraries(
  CTEST_Dxxwjz3p
  DarknetXXNetworksCore
  GTest::gtest_main
)

include(GoogleTest)

gtest_add_tests(
  TARGET CTEST_Dxxwjz3p
  TEST_SUFFIX .noArgs
  TEST_LIST noArgsTests
)

set_tests_properties(${noArgsTests} PROPERTIES TIMEOUT 600)
