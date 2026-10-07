cmake_minimum_required(VERSION 3.13)

darknetxx_targets_sources(
  DARKNETXX_APP_TARGETS
  ${CMAKE_CURRENT_SOURCE_DIR}/ApplicationContext2LibraryContext.cpp
)

add_subdirectory(tests)
