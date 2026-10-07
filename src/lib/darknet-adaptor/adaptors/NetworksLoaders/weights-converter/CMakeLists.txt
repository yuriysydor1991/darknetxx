cmake_minimum_required(VERSION 3.13)

# The conversion of the network weights files between the formats: the
# original darknet weights, the dxxwjz1 and the dxxwjz2 ones.
add_library(
  DarknetCXXWeightsConverterOBJ OBJECT
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
