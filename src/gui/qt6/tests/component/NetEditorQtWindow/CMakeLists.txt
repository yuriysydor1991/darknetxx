cmake_minimum_required(VERSION 3.13)

# The window of the Qt 6 network editor binary on the offscreen platform of
# the Qt (of no display, unless the QT_QPA_PLATFORM tells another one).
add_executable(
  CTEST_NetEditorQtWindow
  CTEST_NetEditorQtWindow.cpp
)

target_compile_definitions(
  CTEST_NetEditorQtWindow
  PRIVATE
    CTEST_NetEditorQtWindow_DATA_DIR=\"${CMAKE_CURRENT_BINARY_DIR}\"
    QT_NO_KEYWORDS
)

target_include_directories(
  CTEST_NetEditorQtWindow
  PRIVATE
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_BINARY_DIR}
)

target_link_libraries(
  CTEST_NetEditorQtWindow
  DarknetXXQt6NetEditorWindowOBJ
  DarknetXXNetEditorOBJ
  DarknetXXDefaultLoggerObj
  ${DARKNETXX_LIBRARY_NAME}
  Qt6::Widgets
  Qt6::Svg
  GTest::gtest_main
)

include(GoogleTest)

gtest_add_tests(
  TARGET CTEST_NetEditorQtWindow
  TEST_SUFFIX .noArgs
  TEST_LIST noArgsTests
)

set_tests_properties(${noArgsTests} PROPERTIES TIMEOUT 600)
