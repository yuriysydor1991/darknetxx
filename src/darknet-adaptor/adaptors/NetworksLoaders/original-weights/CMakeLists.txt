cmake_minimum_required(VERSION 3.13)

# The original darknet weights files: the reader, the writer and the validator
# of them.
add_library(
  DarknetCXXOrigWeightsOBJ OBJECT
  OrigWeightsReader.cpp
  OrigWeightsValidator.cpp
  OrigWeightsWriter.cpp
)

target_include_directories(
  DarknetCXXOrigWeightsOBJ 
  PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}
    ${MOD_DARKNET_INCLUDES}
)

target_link_libraries(
  ${PROJECT_BINARY_NAME}
  DarknetCXXOrigWeightsOBJ
)

add_subdirectory(tests)
