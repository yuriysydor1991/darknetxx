cmake_minimum_required(VERSION 3.13)

if(NOT ENABLE_COMPONENT_TESTS)
  return()
endif()

add_subdirectory(Yolo4Folder)
add_subdirectory(BackgroundTrainer)
