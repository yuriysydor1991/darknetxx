cmake_minimum_required(VERSION 3.13)

add_executable(
  UTEST_WeightsConversionLib
  UTEST_WeightsConversionLib.cpp
  ${PROJECT_SOURCE_DIR}/src/lib/libmain/WeightsConversionLib.cpp
  ${PROJECT_SOURCE_DIR}/src/lib/facade/LibraryContext.cpp
)

target_include_directories(
  UTEST_WeightsConversionLib
  PRIVATE ${LIBMAIN_MOCKS_ROOT}/WeightsConverter
  PRIVATE ${GENERAL_MOCKS_ROOT}/log
  PRIVATE ${PROJECT_SOURCE_DIR}
)

target_link_libraries(
  UTEST_WeightsConversionLib
  GTest::gtest_main GTest::gmock
)

include(GoogleTest)

gtest_add_tests(
  TARGET UTEST_WeightsConversionLib
  TEST_SUFFIX .noArgs
  TEST_LIST noArgsTests
)

set_tests_properties(${noArgsTests} PROPERTIES TIMEOUT 600)
