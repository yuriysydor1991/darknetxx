cmake_minimum_required(VERSION 3.13)

# The original darknet weights files: the reader, the writer and the validator
# of them.
add_library(
  DarknetCXXOrigWeightsOBJ OBJECT
  OrigWeightsFileFormat.cpp
  OrigWeightsReader.cpp
  OrigWeightsValidator.cpp
  OrigWeightsWriter.cpp
)

target_include_directories(
  DarknetCXXOrigWeightsOBJ 
  PRIVATE
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_BINARY_DIR}
    ${MOD_DARKNET_INCLUDES}
)

add_subdirectory(tests)
