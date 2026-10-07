cmake_minimum_required(VERSION 3.13)

# The chart of the training record the weights files keep.
add_library(
  DarknetCXXTrainingChartOBJ OBJECT
  TrainingChart.cpp
)

target_include_directories(
  DarknetCXXTrainingChartOBJ
  PRIVATE
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_BINARY_DIR}
    ${MOD_DARKNET_INCLUDES}
    ${OpenCV_INCLUDE_DIRS}
)

add_subdirectory(tests)
