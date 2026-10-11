cmake_minimum_required(VERSION 3.13)

add_library(
  DarknetCXXALayerAdaptorOBJ OBJECT
  ALayerAdaptor.cpp
)

target_include_directories(
  DarknetCXXALayerAdaptorOBJ 
  PRIVATE
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_BINARY_DIR}
    ${MOD_DARKNET_INCLUDES}
)

add_subdirectory(tests)
