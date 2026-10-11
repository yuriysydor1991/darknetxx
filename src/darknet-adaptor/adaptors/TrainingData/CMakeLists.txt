cmake_minimum_required(VERSION 3.13)

# The image lists and the labels of the training of a dxxwjz2p project, which
# the darknet C code reads instead of the list and the label files.
add_library(
  DarknetCXXTrainingDataOBJ OBJECT
  TrainingDataCApi.cpp
  TrainingDataRegistry.cpp
)

target_include_directories(
  DarknetCXXTrainingDataOBJ
  PRIVATE
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_BINARY_DIR}
    ${MOD_DARKNET_INCLUDES}
)

add_subdirectory(tests)
