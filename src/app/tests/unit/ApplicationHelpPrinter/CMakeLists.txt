cmake_minimum_required(VERSION 3.13)

add_executable(
  UTEST_ApplicationHelpPrinter  
  UTEST_ApplicationHelpPrinter.cpp
  ${PROJECT_SOURCE_DIR}/src/app/ApplicationHelpPrinter.cpp
  ${APP_MOCKS_ROOT}/ApplicationContext/src/app/ApplicationContext.cpp
)

# before the public library headers, which the facade mock replaces
target_include_directories(
  UTEST_ApplicationHelpPrinter
  BEFORE PRIVATE
    ${APP_MOCKS_ROOT}/ApplicationContext
    ${APP_MOCKS_ROOT}/LibraryFacade
    ${GENERAL_MOCKS_ROOT}/project-global-decls
    ${GENERAL_MOCKS_ROOT}/log
    ${PROJECT_SOURCE_DIR}
)

target_link_libraries(
  UTEST_ApplicationHelpPrinter
  GTest::gtest_main GTest::gmock
)

include(GoogleTest)

gtest_add_tests(
  TARGET UTEST_ApplicationHelpPrinter
  TEST_SUFFIX .noArgs
  TEST_LIST noArgsTests
)

set_tests_properties(${noArgsTests} PROPERTIES TIMEOUT 600)
