cmake_minimum_required(VERSION 3.13)

# The darknet network loader of the cfg sections (the ones of the
# darknetxx-datadrivers library CfgSection) and of the weights files of the
# library formats.
add_library(
  DarknetCXXNetworkLoaderOrigDefaultOBJ OBJECT
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
