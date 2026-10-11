cmake_minimum_required(VERSION 3.13)

add_library(
  DarknetXXTrainingOBJ OBJECT
  ATrainingProject.cpp
  BackgroundTrainer.cpp
  DxxwjzProject.cpp
  Yolo4Folder.cpp
)

target_include_directories(
  DarknetXXTrainingOBJ
  PRIVATE
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_BINARY_DIR}
)

# the include directories and the definitions of the weights file formats
target_link_libraries(
  DarknetXXTrainingOBJ
  PRIVATE DarknetCXXModDarknetAdaptorOBJ
)

add_subdirectory(tests)
