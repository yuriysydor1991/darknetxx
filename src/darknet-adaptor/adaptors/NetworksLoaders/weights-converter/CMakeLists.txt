cmake_minimum_required(VERSION 3.13)

# The conversion of the network weights files between the formats: the
# original darknet weights and the dxxwjz1 ones.
add_library(
  DarknetCXXWeightsConverterOBJ OBJECT
  WeightsConverter.cpp
)

target_include_directories(
  DarknetCXXWeightsConverterOBJ
  PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}
    ${MOD_DARKNET_INCLUDES}
)

target_link_libraries(
  ${PROJECT_BINARY_NAME}
  DarknetCXXWeightsConverterOBJ
)

add_subdirectory(tests)
