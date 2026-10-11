cmake_minimum_required(VERSION 3.13)

add_executable(
  CTEST_WeightsConverter
  CTEST_WeightsConverter.cpp
)

# the generated cfg, weights and dxxwjz1 files of the tests, every test
# removes its own ones
target_compile_definitions(
  CTEST_WeightsConverter
  PRIVATE
    CTEST_WeightsConverter_DATA_DIR=\"${CMAKE_CURRENT_BINARY_DIR}/generated\"
    CTEST_WeightsConverter_REFERENCE_DIR=\"${PROJECT_SOURCE_DIR}/src/darknet-adaptor/adaptors/NetworksLoaders/OrigDefaultLoader/tests/component/OrigDefaultLoader/data\"
)

target_link_libraries(
  CTEST_WeightsConverter
  DarknetXXNetworksCore
  DarknetXXWeightsFactory
  GTest::gtest_main GTest::gmock
)

include(GoogleTest)

gtest_add_tests(
  TARGET CTEST_WeightsConverter
  TEST_SUFFIX .noArgs
  TEST_LIST noArgsTests
)

set_tests_properties(${noArgsTests} PROPERTIES TIMEOUT 1800)
