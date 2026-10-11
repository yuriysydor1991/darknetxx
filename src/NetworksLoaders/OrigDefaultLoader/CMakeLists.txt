cmake_minimum_required(VERSION 3.13)

add_library(
  DarknetCXXNetworkLoaderOrigDefaultOBJ OBJECT
  CfgSection.cpp
  OrigDefaultLoader.cpp
)

target_include_directories(
  DarknetCXXNetworkLoaderOrigDefaultOBJ 
  PRIVATE
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_BINARY_DIR}
    ${MOD_DARKNET_INCLUDES}
)

add_subdirectory(tests)
