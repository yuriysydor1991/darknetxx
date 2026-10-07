cmake_minimum_required(VERSION 3.13)

darknetxx_targets_sources(
  DARKNETXX_APP_TARGETS
  ${CMAKE_CURRENT_SOURCE_DIR}/ContinueSignalsHandler.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/IgnoredSignalsHandler.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/PauseSignalsHandler.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/PosixSignalsHandler.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/ReloadSignalsHandler.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/StopSignalsHandler.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/UserSignalsHandler.cpp
)

add_subdirectory(tests)
