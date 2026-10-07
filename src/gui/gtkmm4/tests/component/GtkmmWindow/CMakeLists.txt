cmake_minimum_required(VERSION 3.13)

# The window of the GUI binary over a real training. The tests skip themselves
# without a display to show the window on.
add_executable(
  CTEST_GtkmmWindow
  CTEST_GtkmmWindow.cpp
  ${PROJECT_SOURCE_DIR}/src/app/ApplicationContext.cpp
)

target_compile_definitions(
  CTEST_GtkmmWindow
  PRIVATE CTEST_GtkmmWindow_YOLO4_DIR=\"${DARKNETXX_TESTS_YOLO4_DIR}\"
  PRIVATE CTEST_GtkmmWindow_DATA_DIR=\"${CMAKE_CURRENT_BINARY_DIR}\"
)

target_include_directories(
  CTEST_GtkmmWindow
  PRIVATE
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_BINARY_DIR}
)

target_link_libraries(
  CTEST_GtkmmWindow
  DarknetXXGtkmm4OBJ
  DarknetXXGtkmm4WindowOBJ
  DarknetXXGtkmm4WindowEventHandlersOBJ
  DarknetXXGuiResourcesOBJ
  DarknetXXDefaultLoggerObj
  ${DARKNETXX_LIBRARY_NAME}
  PkgConfig::GTKMM
  GTest::gtest_main
)

include(GoogleTest)

gtest_add_tests(
  TARGET CTEST_GtkmmWindow
  TEST_SUFFIX .noArgs
  TEST_LIST noArgsTests
)

set_tests_properties(${noArgsTests} PROPERTIES TIMEOUT 1800)

if (ENABLE_SANITIZERS_THREADS)
  set_tests_properties(
    ${noArgsTests}
    PROPERTIES
      ENVIRONMENT "TSAN_OPTIONS=suppressions=${PROJECT_SOURCE_DIR}/misc/.tsan-suppress"
  )
endif()
