cmake_minimum_required(VERSION 3.13)

# The app::IApplication descendants the app::ApplicationFactory creates, every
# executable's; the GUI ones add their own (see the
# src/darknetxx-gui-targets.cmake).
darknetxx_targets_sources(
  DARKNETXX_APP_TARGETS
  ${CMAKE_CURRENT_SOURCE_DIR}/Application.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/ApplicationHelpPrinter.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/ApplicationVersionPrinter.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/NetworkCfgPrinter.cpp
)

add_subdirectory(tests)
