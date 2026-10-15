cmake_minimum_required(VERSION 3.13)

if(NOT ENABLE_COMPONENT_TESTS)
  return()
endif()

if(TARGET ${DARKNETXX_TRAIN_GUI_BINARY_NAME})
  add_subdirectory(TrainWindow)
  add_subdirectory(RecognitionWindow)
endif()

if(TARGET ${DARKNETXX_NET_EDITOR_GUI_BINARY_NAME})
  add_subdirectory(NetEditorWindow)
endif()
