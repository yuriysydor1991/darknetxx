cmake_minimum_required(VERSION 3.13)

# The ApplicationFactory of the GUI binary: the very same source compiled with
# the DARKNETXX_GUI definition, which creates the app::GuiApplication.
add_executable(
  UTEST_GuiApplicationFactory
  UTEST_GuiApplicationFactory.cpp
  ${PROJECT_SOURCE_DIR}/src/app/ApplicationFactory.cpp
  ${APP_MOCKS_ROOT}/ApplicationContext/src/app/ApplicationContext.cpp
)

target_compile_definitions(
  UTEST_GuiApplicationFactory
  PRIVATE DARKNETXX_GUI
)

# before the public library headers, which the facade mock replaces
target_include_directories(
  UTEST_GuiApplicationFactory
  BEFORE PRIVATE
    ${APP_MOCKS_ROOT}/ApplicationContext
    ${APP_MOCKS_ROOT}/Application
    ${APP_MOCKS_ROOT}/GuiApplication
    ${APP_MOCKS_ROOT}/ApplicationHelpPrinter
    ${APP_MOCKS_ROOT}/ApplicationVersionPrinter
    ${APP_MOCKS_ROOT}/LibraryFacade
    ${APP_MOCKS_ROOT}/CommandLineParser
    ${SIGNALS_HANDLERS_MOCKS_ROOT}/SignalsHandler
    ${GENERAL_MOCKS_ROOT}/project-global-decls
    ${GENERAL_MOCKS_ROOT}/log
    ${PROJECT_SOURCE_DIR}
)

target_link_libraries(
  UTEST_GuiApplicationFactory
  GTest::gtest_main GTest::gmock
)

include(GoogleTest)

gtest_add_tests(
  TARGET UTEST_GuiApplicationFactory
  TEST_SUFFIX .noArgs
  TEST_LIST noArgsTests
)

set_tests_properties(${noArgsTests} PROPERTIES TIMEOUT 600)
