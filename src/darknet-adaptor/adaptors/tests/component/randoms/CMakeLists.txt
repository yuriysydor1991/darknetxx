cmake_minimum_required(VERSION 3.13)

add_executable(
  CTEST_darknet_randoms  
  CTEST_darknet_randoms.cpp
  ${MOD_DARKNET_SRC_ROOT}/src/randoms.c
  ${MOD_DARKNET_SRC_ROOT}/src/memory.c
  ${MOD_DARKNET_SRC_ROOT}/src/string_tools.c
  ${MOD_DARKNET_SRC_ROOT}/src/error.c
  ${MOD_DARKNET_SRC_ROOT}/src/clog/clog.c
)

target_include_directories(
  CTEST_darknet_randoms
  PRIVATE 
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}
    ${MOD_DARKNET_INCLUDES}
)

target_link_libraries(
  CTEST_darknet_randoms
  GTest::gtest_main GTest::gmock 
  DarknetXXLog4CInterfaceObj 
  TemplateProjectDefaultLoggerObj
)

include(GoogleTest)

gtest_add_tests(
  TARGET CTEST_darknet_randoms
  TEST_SUFFIX .noArgs
  TEST_LIST noArgsTests
)

set_tests_properties(${noArgsTests} PROPERTIES TIMEOUT 600)

