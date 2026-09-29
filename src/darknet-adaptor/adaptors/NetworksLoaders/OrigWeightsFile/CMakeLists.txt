cmake_minimum_required(VERSION 3.13)

add_library(
  DarknetCXXOrigWeightsFileOBJ OBJECT
  OrigWeightsFile.cpp
)

target_include_directories(
  DarknetCXXOrigWeightsFileOBJ 
  PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}
    ${MOD_DARKNET_INCLUDES}
)

target_link_libraries(
  ${PROJECT_BINARY_NAME}
  DarknetCXXOrigWeightsFileOBJ
)

add_subdirectory(tests)
