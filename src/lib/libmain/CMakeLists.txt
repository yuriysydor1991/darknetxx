cmake_minimum_required(VERSION 3.13)

add_library(
  DarknetXXLibMainOBJ OBJECT
  LibFactory.cpp
  LibMain.cpp
  WeightsConversionLib.cpp
)

# the include directories and the definitions of the DarknetController
target_link_libraries(
  DarknetXXLibMainOBJ
  PRIVATE DarknetCXXModDarknetAdaptorOBJ
)

add_subdirectory(tests)
