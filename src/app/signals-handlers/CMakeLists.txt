cmake_minimum_required(VERSION 3.13)

darknetxx_targets_sources(
  DARKNETXX_APP_TARGETS
  ${CMAKE_CURRENT_SOURCE_DIR}/SignalsHandler.cpp
)

add_subdirectory(handlers)
add_subdirectory(tests)
