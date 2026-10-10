cmake_minimum_required(VERSION 3.13)

add_executable(
  CTEST_Yolo4Folder
  CTEST_Yolo4Folder.cpp
  ${PROJECT_SOURCE_DIR}/src/lib/training/ATrainingProject.cpp
  ${PROJECT_SOURCE_DIR}/src/lib/training/Yolo4Folder.cpp
)

# the folders the tests create, every test removes it's own one
target_compile_definitions(
  CTEST_Yolo4Folder
  PRIVATE CTEST_Yolo4Folder_DATA_DIR=\"${CMAKE_CURRENT_BINARY_DIR}\"
)

target_link_libraries(
  CTEST_Yolo4Folder
  DarknetXXNetworksCore
  GTest::gtest_main
)

include(GoogleTest)

gtest_add_tests(
  TARGET CTEST_Yolo4Folder
  TEST_SUFFIX .noArgs
  TEST_LIST noArgsTests
)

set_tests_properties(${noArgsTests} PROPERTIES TIMEOUT 600)
