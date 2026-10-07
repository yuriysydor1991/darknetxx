cmake_minimum_required(VERSION 3.13)

# The network weights file formats: the original darknet weights, the dxxwjz1
# and the dxxwjz2 ones, found by their names and their file name extensions,
# and the files networks are made of: the cfg and the dxxwjz2 ones.
add_library(
  DarknetCXXWeightsFormatsOBJ OBJECT
  NetworkSource.cpp
  WeightsFileFormats.cpp
)

target_include_directories(
  DarknetCXXWeightsFormatsOBJ
  PRIVATE
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_BINARY_DIR}
    ${MOD_DARKNET_INCLUDES}
)

add_subdirectory(tests)
