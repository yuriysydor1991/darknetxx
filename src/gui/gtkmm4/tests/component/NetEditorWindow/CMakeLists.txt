cmake_minimum_required(VERSION 3.13)

# The window of the GTKmm 4 network editor binary. The tests skip themselves
# without a display to show the window on.
add_executable(
  CTEST_NetEditorWindow
  CTEST_NetEditorWindow.cpp
  ${PROJECT_SOURCE_DIR}/src/app/ApplicationContext.cpp
)

target_compile_definitions(
  CTEST_NetEditorWindow
  PRIVATE CTEST_NetEditorWindow_DATA_DIR=\"${CMAKE_CURRENT_BINARY_DIR}\"
)

target_include_directories(
  CTEST_NetEditorWindow
  PRIVATE
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_BINARY_DIR}
)

target_link_libraries(
  CTEST_NetEditorWindow
  DarknetXXGtkmm4OBJ
  DarknetXXGtkmm4NetEditorWindowOBJ
  DarknetXXNetEditorOBJ
  DarknetXXGuiResourcesOBJ
  DarknetXXDefaultLoggerObj
  ${DARKNETXX_LIBRARY_NAME}
  PkgConfig::GTKMM
  GTest::gtest_main
)

if(TARGET DarknetXXFontAwesomeGResourceOBJ)
  target_link_libraries(CTEST_NetEditorWindow DarknetXXFontAwesomeGResourceOBJ)
endif()

include(GoogleTest)

gtest_add_tests(
  TARGET CTEST_NetEditorWindow
  TEST_SUFFIX .noArgs
  TEST_LIST noArgsTests
)

set_tests_properties(${noArgsTests} PROPERTIES TIMEOUT 600)
