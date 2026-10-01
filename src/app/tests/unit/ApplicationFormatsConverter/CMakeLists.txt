cmake_minimum_required(VERSION 3.13)

add_executable(
  UTEST_ApplicationFormatsConverter
  UTEST_ApplicationFormatsConverter.cpp
  ${CMAKE_SOURCE_DIR}/src/app/converter/ApplicationFormatsConverter.cpp
  ${CMAKE_SOURCE_DIR}/src/app/converter/ConversionFormats.cpp
  ${APP_MOCKS_ROOT}/ApplicationContext/src/app/ApplicationContext.cpp
)

target_include_directories(
  UTEST_ApplicationFormatsConverter
  PRIVATE ${APP_MOCKS_ROOT}/ApplicationContext
  PRIVATE ${APP_MOCKS_ROOT}/WeightsConverter
  PRIVATE ${APP_MOCKS_ROOT}/log
  PRIVATE ${CMAKE_SOURCE_DIR}
)

target_link_libraries(
  UTEST_ApplicationFormatsConverter
  GTest::gtest_main GTest::gmock
)

include(GoogleTest)

gtest_add_tests(
  TARGET UTEST_ApplicationFormatsConverter
  TEST_SUFFIX .noArgs
  TEST_LIST noArgsTests
)

set_tests_properties(${noArgsTests} PROPERTIES TIMEOUT 600)
