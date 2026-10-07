cmake_minimum_required(VERSION 3.13)

add_library(
  DarknetXXGtkmm4WindowOBJ OBJECT
  WindowEventsHandler.cpp
  WindowLoader.cpp
)

target_include_directories(
  DarknetXXGtkmm4WindowOBJ
  PRIVATE
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_BINARY_DIR}
)

target_link_libraries(
  DarknetXXGtkmm4WindowOBJ
  PRIVATE PkgConfig::GTKMM
)

target_link_libraries(
  ${DARKNETXX_GUI_BINARY_NAME}
  DarknetXXGtkmm4WindowOBJ
)

add_subdirectory(event-handlers)
