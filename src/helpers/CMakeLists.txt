cmake_minimum_required(VERSION 3.13)

add_library(
  DarknetCXXHelpersComponentOBJ OBJECT
  StringTools.cpp
)

target_include_directories(
  DarknetCXXHelpersComponentOBJ 
  PUBLIC
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_BINARY_DIR}
)
