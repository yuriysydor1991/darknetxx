cmake_minimum_required(VERSION 3.13)

# The ApplicationFactory of the network weights files converter only binary:
# the very same source compiled with the DARKNETXX_CONVERTER_ONLY definition,
# which leaves the app::Application class out.
add_executable(
  UTEST_ConverterOnlyApplicationFactory
  UTEST_ConverterOnlyApplicationFactory.cpp
  ${CMAKE_SOURCE_DIR}/src/app/ApplicationFactory.cpp
  ${APP_MOCKS_ROOT}/ApplicationContext/src/app/ApplicationContext.cpp
)

target_compile_definitions(
  UTEST_ConverterOnlyApplicationFactory
  PRIVATE DARKNETXX_CONVERTER_ONLY
)

target_include_directories(
  UTEST_ConverterOnlyApplicationFactory
  PRIVATE ${APP_MOCKS_ROOT}/ApplicationContext
  PRIVATE ${APP_MOCKS_ROOT}/ApplicationHelpPrinter
  PRIVATE ${APP_MOCKS_ROOT}/ApplicationVersionPrinter
  PRIVATE ${APP_MOCKS_ROOT}/ApplicationConverterComponent
  PRIVATE ${APP_MOCKS_ROOT}/ApplicationFormatsConverter
  PRIVATE ${APP_MOCKS_ROOT}/CommandLineParser
  PRIVATE ${SIGNALS_HANDLERS_MOCKS_ROOT}/SignalsHandler
  PRIVATE ${APP_MOCKS_ROOT}/project-global-decls
  PRIVATE ${APP_MOCKS_ROOT}/log
  PRIVATE ${CMAKE_SOURCE_DIR}
)

target_link_libraries(
  UTEST_ConverterOnlyApplicationFactory
  GTest::gtest_main GTest::gmock
)

include(GoogleTest)

gtest_add_tests(
  TARGET UTEST_ConverterOnlyApplicationFactory
  TEST_SUFFIX .noArgs
  TEST_LIST noArgsTests
)

set_tests_properties(${noArgsTests} PROPERTIES TIMEOUT 600)
