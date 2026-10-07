cmake_minimum_required(VERSION 3.13)

add_executable(
  UTEST_GuiApplication
  UTEST_GuiApplication.cpp
  ${PROJECT_SOURCE_DIR}/src/app/Application.cpp
  ${PROJECT_SOURCE_DIR}/src/app/GuiApplication.cpp
  ${PROJECT_SOURCE_DIR}/src/lib/facade/LibraryContext.cpp
  ${APP_MOCKS_ROOT}/ApplicationContext/src/app/ApplicationContext.cpp
)

# before the public library headers, which the facade mock replaces
target_include_directories(
  UTEST_GuiApplication
  BEFORE PRIVATE
    ${APP_MOCKS_ROOT}/ApplicationContext
    ${APP_MOCKS_ROOT}/LibraryFacade
    ${APP_MOCKS_ROOT}/ApplicationContext2LibraryContext
    ${APP_MOCKS_ROOT}/GtkmmIniter
    ${GENERAL_MOCKS_ROOT}/project-global-decls
    ${GENERAL_MOCKS_ROOT}/log
    ${PROJECT_SOURCE_DIR}
)

target_link_libraries(
  UTEST_GuiApplication
  GTest::gtest_main GTest::gmock
)

include(GoogleTest)

gtest_add_tests(
  TARGET UTEST_GuiApplication
  TEST_SUFFIX .noArgs
  TEST_LIST noArgsTests
)

set_tests_properties(${noArgsTests} PROPERTIES TIMEOUT 600)
