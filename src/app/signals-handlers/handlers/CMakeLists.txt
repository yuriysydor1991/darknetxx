cmake_minimum_required(VERSION 3.13)

darknetxx_targets_sources(
  DARKNETXX_APP_TARGETS
  ${CMAKE_CURRENT_SOURCE_DIR}/ASignalsHandler.cpp
)

add_subdirectory(posix)
add_subdirectory(tests)
