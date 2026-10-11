cmake_minimum_required(VERSION 3.13)

add_executable(
  CTEST_Yolo4FolderFormat
  CTEST_Yolo4FolderFormat.cpp
)

# the digits YOLO4 folder (see the src/tests/digits-yolo4-folder.cmake), the
# folders the tests create, every test removes it's own one
target_compile_definitions(
  CTEST_Yolo4FolderFormat
  PRIVATE CTEST_Yolo4FolderFormat_YOLO4_DIR=\"${DARKNETXX_TESTS_YOLO4_DIR}\"
  PRIVATE CTEST_Yolo4FolderFormat_DATA_DIR=\"${CMAKE_CURRENT_BINARY_DIR}\"
)

target_link_libraries(
  CTEST_Yolo4FolderFormat
  DarknetXXNetworksCore
  GTest::gtest_main
)

include(GoogleTest)

gtest_add_tests(
  TARGET CTEST_Yolo4FolderFormat
  TEST_SUFFIX .noArgs
  TEST_LIST noArgsTests
)

set_tests_properties(${noArgsTests} PROPERTIES TIMEOUT 600)
