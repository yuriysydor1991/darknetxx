cmake_minimum_required(VERSION 3.13)

add_executable(
  CTEST_WeightsModel
  CTEST_WeightsModel.cpp
  CTEST_WeightsModel_layouts.cpp
)

# the generated cfg files of the tests, every test removes its own ones
target_compile_definitions(
  CTEST_WeightsModel
  PRIVATE CTEST_WeightsModel_DATA_DIR=\"${CMAKE_CURRENT_BINARY_DIR}/generated\"
)

target_link_libraries(
  CTEST_WeightsModel
  DarknetXXNetworksCore
  DarknetXXWeightsFactory
  GTest::gtest_main GTest::gmock
)

include(GoogleTest)

gtest_add_tests(
  TARGET CTEST_WeightsModel
  TEST_SUFFIX .noArgs
  TEST_LIST noArgsTests
)

set_tests_properties(${noArgsTests} PROPERTIES TIMEOUT 1800)
