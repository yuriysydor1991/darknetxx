cmake_minimum_required(VERSION 3.13)

# The window of the GUI recognition binary over the networks of no features
# computed (see the src/tests/component/bias-networks directory). The tests
# skip themselves without a display to show the window on.
add_executable(
  CTEST_RecognitionWindow
  CTEST_RecognitionWindow.cpp
  ${PROJECT_SOURCE_DIR}/src/app/ApplicationContext.cpp
)

target_compile_definitions(
  CTEST_RecognitionWindow
  PRIVATE CTEST_RecognitionWindow_DATA_DIR=\"${CMAKE_CURRENT_BINARY_DIR}\"
)

target_include_directories(
  CTEST_RecognitionWindow
  PRIVATE
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_BINARY_DIR}
)

target_link_libraries(
  CTEST_RecognitionWindow
  DarknetXXGtkmm4OBJ
  DarknetXXGtkmm4RecognitionWindowOBJ
  DarknetXXGuiResourcesOBJ
  DarknetXXDefaultLoggerObj
  ${DARKNETXX_LIBRARY_NAME}
  PkgConfig::GTKMM
  Threads::Threads
  GTest::gtest_main
)

include(GoogleTest)

gtest_add_tests(
  TARGET CTEST_RecognitionWindow
  TEST_SUFFIX .noArgs
  TEST_LIST noArgsTests
)

set_tests_properties(${noArgsTests} PROPERTIES TIMEOUT 600)
