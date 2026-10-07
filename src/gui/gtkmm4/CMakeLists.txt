cmake_minimum_required(VERSION 3.13)

add_library(
  DarknetXXGtkmm4OBJ OBJECT
  GtkmmIniter.cpp
  MainWindowContext.cpp
)

target_include_directories(
  DarknetXXGtkmm4OBJ
  PRIVATE
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_BINARY_DIR}
)

target_link_libraries(
  DarknetXXGtkmm4OBJ
  PRIVATE PkgConfig::GTKMM
)

target_link_libraries(
  ${DARKNETXX_GUI_BINARY_NAME}
  DarknetXXGtkmm4OBJ
  PkgConfig::GTKMM
)

add_subdirectory(ui)
add_subdirectory(main-window)
add_subdirectory(tests)
