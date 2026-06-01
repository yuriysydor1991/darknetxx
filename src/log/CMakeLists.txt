cmake_minimum_required(VERSION 3.13)

option(
  ENABLE_LOGS_MICROSECONDS_TIME
  "Set to ON to append the microseconds to the DefaultLogger message timestamps"
  OFF
)

if (ENABLE_LOGS_MICROSECONDS_TIME)
  add_compile_definitions(ENABLE_LOGS_MICROSECONDS_TIME=1)
endif()

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

add_subdirectory(default-logger)

add_subdirectory(tests)
