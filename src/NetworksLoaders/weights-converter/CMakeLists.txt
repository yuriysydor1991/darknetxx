cmake_minimum_required(VERSION 3.13)

# The conversion of the network weights files between the formats: the
# original darknet weights, the dxxwjz1, the dxxwjz2 and the dxxwjz3 ones and
# their projects, and the rewrite of the edited network into them.
add_library(
  DarknetCXXWeightsConverterOBJ OBJECT
  NetworkRewriter.cpp
  WeightsConversion.cpp
  WeightsConverter.cpp
)

target_include_directories(
  DarknetCXXWeightsConverterOBJ
  PRIVATE
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_BINARY_DIR}
    ${MOD_DARKNET_INCLUDES}
)

add_subdirectory(tests)
