cmake_minimum_required(VERSION 3.13)

add_library(
  DarknetCXXHelpersComponentOBJ OBJECT
  StringTools.cpp
)

target_include_directories(
  DarknetCXXHelpersComponentOBJ 
  PUBLIC
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}
)

target_link_libraries(
  ${PROJECT_BINARY_NAME}
  DarknetCXXHelpersComponentOBJ
)
