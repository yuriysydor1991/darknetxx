cmake_minimum_required(VERSION 3.13)

add_library(
  DarknetCXXADetectorTrainerAdaptorOBJ OBJECT
  ADetectorTrainer.cpp
)

target_include_directories(
  DarknetCXXADetectorTrainerAdaptorOBJ 
  PRIVATE
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_BINARY_DIR}
    ${MOD_DARKNET_INCLUDES}
)

