cmake_minimum_required(VERSION 3.13)

# the parts every window shares
add_library(
  DarknetXXGtkmm4OBJ OBJECT
  AWindowContext.cpp
  AWindowLoader.cpp
  ChooserEventsHandler.cpp
  GtkmmIniter.cpp
  StopSignalEventsHandler.cpp
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

foreach(GUI_BINARY_NAME ${DARKNETXX_TRAIN_GUI_BINARY_NAME} ${DARKNETXX_RECOGNITION_GUI_BINARY_NAME})
  target_link_libraries(
    ${GUI_BINARY_NAME}
    DarknetXXGtkmm4OBJ
    DarknetXXGuiResourcesOBJ
    PkgConfig::GTKMM
  )
endforeach()

# the recognition window recognizes in a thread of it's own
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_subdirectory(ui)
add_subdirectory(train-window)
add_subdirectory(recognition-window)
add_subdirectory(tests)
