cmake_minimum_required(VERSION 3.13)

add_library(
  DarknetCXXADetectorAdaptorOBJ OBJECT
  ADetector.cpp
)

target_include_directories(
  DarknetCXXADetectorAdaptorOBJ 
  PRIVATE
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_BINARY_DIR}
    ${MOD_DARKNET_INCLUDES}
)

