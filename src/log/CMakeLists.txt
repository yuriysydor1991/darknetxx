cmake_minimum_required(VERSION 3.13)

add_library(
  DarknetXXLog4CInterfaceObj OBJECT
  cpplog4c.cpp
)

target_include_directories(
  DarknetXXLog4CInterfaceObj 
  PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}
)

target_link_libraries(
  ${PROJECT_BINARY_NAME}
  DarknetXXLog4CInterfaceObj
)

add_subdirectory(simple-logger)

