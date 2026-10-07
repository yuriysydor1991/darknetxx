cmake_minimum_required(VERSION 3.13)

add_library(
  DarknetXXDefaultLoggerObj OBJECT
  DefaultLogger.cpp
  real-default-logger/RealDefaultLogger.cpp
)

target_include_directories(
  DarknetXXDefaultLoggerObj
  PRIVATE
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_BINARY_DIR}
)

add_subdirectory(tests)
