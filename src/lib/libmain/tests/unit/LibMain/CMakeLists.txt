cmake_minimum_required(VERSION 3.13)

add_executable(
  UTEST_LibMain
  UTEST_LibMain.cpp
  ${PROJECT_SOURCE_DIR}/src/lib/libmain/LibMain.cpp
  ${PROJECT_SOURCE_DIR}/src/lib/facade/LibraryContext.cpp
  ${LIBMAIN_MOCKS_ROOT}/DarknetController/src/darknet-adaptor/DarknetController.cpp
  ${LIBMAIN_MOCKS_ROOT}/OMPController/src/OMP/OMPController.cpp
)

target_include_directories(
  UTEST_LibMain
  PRIVATE ${LIBMAIN_MOCKS_ROOT}/DarknetController
  PRIVATE ${LIBMAIN_MOCKS_ROOT}/OMPController
  PRIVATE ${GENERAL_MOCKS_ROOT}/log
  PRIVATE ${PROJECT_SOURCE_DIR}
)

target_link_libraries(
  UTEST_LibMain
  GTest::gtest_main GTest::gmock
)

include(GoogleTest)

gtest_add_tests(
  TARGET UTEST_LibMain
  TEST_SUFFIX .noArgs
  TEST_LIST noArgsTests
)

set_tests_properties(${noArgsTests} PROPERTIES TIMEOUT 600)
