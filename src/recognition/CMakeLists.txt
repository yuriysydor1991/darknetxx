cmake_minimum_required(VERSION 3.13)

add_library(
  DarknetXXRecognitionOBJ OBJECT
  ImageClassification.cpp
  NetworkRecognizer.cpp
  ObjectsDetection.cpp
  RecognitionPainter.cpp
)

target_include_directories(
  DarknetXXRecognitionOBJ
  PRIVATE
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_BINARY_DIR}
)

# the include directories and the definitions of the darknet computation
target_link_libraries(
  DarknetXXRecognitionOBJ
  PRIVATE DarknetCXXModDarknetAdaptorOBJ
)

add_subdirectory(tests)
