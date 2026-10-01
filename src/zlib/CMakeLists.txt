cmake_minimum_required(VERSION 3.13)

# The zlib compression controller of the cpp-app-template appZlib branch,
# adapted for the gzip compressed dxxwjz1 network weights files: the in memory
# and the gzip file compression, the gzip files streams.
add_library(
  TemplateProjectZlibControllerObj OBJECT
  GzipFileStreams.cpp
  ZlibController.cpp
)

target_include_directories(
  TemplateProjectZlibControllerObj
  PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}
)

target_link_libraries(
  TemplateProjectZlibControllerObj
  PUBLIC ZLIB::ZLIB
)

target_link_libraries(
  ${PROJECT_BINARY_NAME}
  TemplateProjectZlibControllerObj
  ZLIB::ZLIB
)

add_subdirectory(tests)
