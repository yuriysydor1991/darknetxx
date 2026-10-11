cmake_minimum_required(VERSION 3.13)

# The zlib compression controller of the cpp-app-template appZlib branch,
# adapted for the gzip compressed dxxwjz1 network weights files: the in memory
# and the gzip file compression, the gzip files streams.
add_library(
  DarknetXXZlibControllerObj OBJECT
  GzipFileStreams.cpp
  ZlibController.cpp
)

target_include_directories(
  DarknetXXZlibControllerObj
  PRIVATE
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_BINARY_DIR}
)

target_link_libraries(
  DarknetXXZlibControllerObj
  PUBLIC ZLIB::ZLIB
)

add_subdirectory(tests)
