cmake_minimum_required(VERSION 3.13)

# The network weights file formats: the original darknet weights, the dxxwjz1,
# the dxxwjz2, the dxxwjz2p, the dxxwjz3 and the dxxwjz3p ones, found by their
# names and their file name extensions, the YOLO4 folder one of the
# conversion, and the files networks are made of: the cfg, the dxxwjz2, the
# dxxwjz2p, the dxxwjz3 and the dxxwjz3p ones.
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
