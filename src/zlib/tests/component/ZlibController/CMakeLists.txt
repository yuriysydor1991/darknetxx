cmake_minimum_required(VERSION 3.13)

add_executable(
  CTEST_ZlibController
  CTEST_ZlibController.cpp
  ${PROJECT_SOURCE_DIR}/src/zlib/GzipFileStreams.cpp
  ${PROJECT_SOURCE_DIR}/src/zlib/ZlibController.cpp
  ${PROJECT_SOURCE_DIR}/src/log/default-logger/DefaultLogger.cpp
  ${PROJECT_SOURCE_DIR}/src/log/default-logger/real-default-logger/RealDefaultLogger.cpp
)

# the gzip files of the tests, every test removes its own ones
target_compile_definitions(
  CTEST_ZlibController
  PRIVATE CTEST_ZlibController_DATA_DIR=\"${CMAKE_CURRENT_BINARY_DIR}\"
)

target_include_directories(
  CTEST_ZlibController
  PRIVATE ${PROJECT_SOURCE_DIR}
  PRIVATE ${PROJECT_BINARY_DIR}
)

target_link_libraries(
  CTEST_ZlibController
  GTest::gtest_main GTest::gmock ZLIB::ZLIB
)

include(GoogleTest)

gtest_add_tests(
  TARGET CTEST_ZlibController
  TEST_SUFFIX .noArgs
  TEST_LIST noArgsTests
)

set_tests_properties(${noArgsTests} PROPERTIES TIMEOUT 600)
