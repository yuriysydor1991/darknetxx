cmake_minimum_required(VERSION 3.13)

# the window of the recognition of the images
add_library(
  DarknetXXGtkmm4RecognitionWindowOBJ OBJECT
  RecognitionEventsHandler.cpp
  RecognitionGtkmmIniter.cpp
  RecognitionWindowContext.cpp
  RecognitionWindowLoader.cpp
)

target_include_directories(
  DarknetXXGtkmm4RecognitionWindowOBJ
  PRIVATE
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_BINARY_DIR}
)

target_link_libraries(
  DarknetXXGtkmm4RecognitionWindowOBJ
  PRIVATE PkgConfig::GTKMM Threads::Threads
)

target_link_libraries(
  ${DARKNETXX_RECOGNITION_GUI_BINARY_NAME}
  DarknetXXGtkmm4RecognitionWindowOBJ
  Threads::Threads
)
