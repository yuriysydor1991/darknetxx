cmake_minimum_required(VERSION 3.13)

# The ApplicationFactory of the GUI binaries: the very same source compiled
# with the DARKNETXX_TRAIN_GUI and the DARKNETXX_RECOGNITION_GUI definitions,
# which create the app::TrainGuiApplication and the
# app::RecognitionGuiApplication.
foreach(GUI Train Recognition)
  string(TOUPPER ${GUI} GUI_ID)
  set(UTEST_TARGET UTEST_${GUI}GuiApplicationFactory)

  add_executable(
    ${UTEST_TARGET}
    ${UTEST_TARGET}.cpp
    ${PROJECT_SOURCE_DIR}/src/app/ApplicationFactory.cpp
    ${APP_MOCKS_ROOT}/ApplicationContext/src/app/ApplicationContext.cpp
  )

  target_compile_definitions(
    ${UTEST_TARGET}
    PRIVATE DARKNETXX_${GUI_ID}_GUI
  )

  # before the public library headers, which the facade mock replaces
  target_include_directories(
    ${UTEST_TARGET}
    BEFORE PRIVATE
      ${APP_MOCKS_ROOT}/ApplicationContext
      ${APPLICATIONS_MOCKS_ROOT}/Application
      ${APPLICATIONS_MOCKS_ROOT}/GuiApplications
      ${APPLICATIONS_MOCKS_ROOT}/ApplicationHelpPrinter
      ${APPLICATIONS_MOCKS_ROOT}/ApplicationVersionPrinter
      ${APPLICATIONS_MOCKS_ROOT}/NetworkCfgPrinter
      ${APP_MOCKS_ROOT}/LibraryFacade
      ${APP_MOCKS_ROOT}/CommandLineParser
      ${SIGNALS_HANDLERS_MOCKS_ROOT}/SignalsHandler
      ${GENERAL_MOCKS_ROOT}/project-global-decls
      ${GENERAL_MOCKS_ROOT}/log
      ${PROJECT_SOURCE_DIR}
  )

  target_link_libraries(
    ${UTEST_TARGET}
    GTest::gtest_main GTest::gmock
  )

  include(GoogleTest)

  gtest_add_tests(
    TARGET ${UTEST_TARGET}
    TEST_SUFFIX .noArgs
    TEST_LIST noArgsTests
  )

  set_tests_properties(${noArgsTests} PROPERTIES TIMEOUT 600)
endforeach()
