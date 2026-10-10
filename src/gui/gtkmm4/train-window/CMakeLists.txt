cmake_minimum_required(VERSION 3.13)

# the window of the YOLO4 folder training
add_library(
  DarknetXXGtkmm4TrainWindowOBJ OBJECT
  TrainGtkmmIniter.cpp
  TrainWindowContext.cpp
  TrainWindowLoader.cpp
  TrainingEventsHandler.cpp
  WindowSystemEventsHandler.cpp
)

target_include_directories(
  DarknetXXGtkmm4TrainWindowOBJ
  PRIVATE
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_BINARY_DIR}
)

target_link_libraries(
  DarknetXXGtkmm4TrainWindowOBJ
  PRIVATE PkgConfig::GTKMM
)

target_link_libraries(
  ${DARKNETXX_TRAIN_GUI_BINARY_NAME}
  DarknetXXGtkmm4TrainWindowOBJ
)
