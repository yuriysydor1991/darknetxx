cmake_minimum_required(VERSION 3.13)

# The window of the GUI training binary over a real training. The tests skip
# themselves without a display to show the window on.
add_executable(
  CTEST_TrainWindow
  CTEST_TrainWindow.cpp
  ${PROJECT_SOURCE_DIR}/src/app/ApplicationContext.cpp
)

target_compile_definitions(
  CTEST_TrainWindow
  PRIVATE CTEST_TrainWindow_YOLO4_DIR=\"${DARKNETXX_TESTS_YOLO4_DIR}\"
  PRIVATE CTEST_TrainWindow_DATA_DIR=\"${CMAKE_CURRENT_BINARY_DIR}\"
)

target_include_directories(
  CTEST_TrainWindow
  PRIVATE
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_BINARY_DIR}
)

target_link_libraries(
  CTEST_TrainWindow
  DarknetXXGtkmm4OBJ
  DarknetXXGtkmm4TrainWindowOBJ
  DarknetXXGuiResourcesOBJ
  DarknetXXDefaultLoggerObj
  ${DARKNETXX_LIBRARY_NAME}
  PkgConfig::GTKMM
  GTest::gtest_main
)

include(GoogleTest)

gtest_add_tests(
  TARGET CTEST_TrainWindow
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
