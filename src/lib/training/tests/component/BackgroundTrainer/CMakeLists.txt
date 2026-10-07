cmake_minimum_required(VERSION 3.13)

add_executable(
  CTEST_BackgroundTrainer
  CTEST_BackgroundTrainer.cpp
)

target_compile_definitions(
  CTEST_BackgroundTrainer
  PRIVATE CTEST_BackgroundTrainer_YOLO4_DIR=\"${DARKNETXX_TESTS_YOLO4_DIR}\"
  PRIVATE CTEST_BackgroundTrainer_DATA_DIR=\"${CMAKE_CURRENT_BINARY_DIR}\"
)

target_link_libraries(
  CTEST_BackgroundTrainer
  DarknetXXLibraryObjects
  GTest::gtest_main
)

include(GoogleTest)

gtest_add_tests(
  TARGET CTEST_BackgroundTrainer
  TEST_SUFFIX .noArgs
  TEST_LIST noArgsTests
)

set_tests_properties(${noArgsTests} PROPERTIES TIMEOUT 1800)

# The ThreadSanitizer reports the known race of the yolo layer statistics
# counter, which the misc/.tsan-suppress file suppresses alone.
if (ENABLE_SANITIZERS_THREADS)
  set_tests_properties(
    ${noArgsTests}
    PROPERTIES
      ENVIRONMENT "TSAN_OPTIONS=suppressions=${PROJECT_SOURCE_DIR}/misc/.tsan-suppress"
  )
endif()
