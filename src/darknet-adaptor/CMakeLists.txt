cmake_minimum_required(VERSION 3.13)

add_library(
  DarknetCXXOrigDarknetAdaptorOBJ OBJECT
  DarknetController.cpp
  DarknetContext.cpp
)

target_include_directories(
  DarknetCXXOrigDarknetAdaptorOBJ 
  PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}
)

target_link_libraries(
  ${PROJECT_BINARY_NAME}
  DarknetCXXOrigDarknetAdaptorOBJ
)

add_subdirectory(adaptors)
