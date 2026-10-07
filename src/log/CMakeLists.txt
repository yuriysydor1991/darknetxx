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
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_BINARY_DIR}
)

add_subdirectory(default-logger)

add_subdirectory(tests)
