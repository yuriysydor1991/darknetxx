cmake_minimum_required(VERSION 3.13)

add_library(
  DarknetCXXOpenMPControllerOBJ OBJECT
  OMPController.cpp
)

target_include_directories(
  DarknetCXXOpenMPControllerOBJ 
  PUBLIC
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}
)

target_link_libraries(
  ${PROJECT_BINARY_NAME}
  DarknetCXXOpenMPControllerOBJ
)

