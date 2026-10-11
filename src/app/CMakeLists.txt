cmake_minimum_required(VERSION 3.13)

# Exposed to the whole src/app component, so every subcomponent test reaches
# the mocks of the neighbouring ones.
set(APP_MOCKS_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/tests/mocks")
set(APPLICATIONS_MOCKS_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/applications/tests/mocks")
set(SIGNALS_HANDLERS_MOCKS_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/signals-handlers/tests/mocks")
set(POSIX_SIGNALS_HANDLERS_MOCKS_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/signals-handlers/handlers/posix/tests/mocks")

# The sources every executable is built of: the main one, the network weights
# files converter only one (see the src/darknetxxConverter-target.cmake) and
# the GUI ones (see the src/darknetxx-gui-targets.cmake), which compile them
# with their own definitions.
darknetxx_targets_sources(
  DARKNETXX_APP_TARGETS
  ${CMAKE_CURRENT_SOURCE_DIR}/ApplicationContext.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/ApplicationFactory.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/CommandLineParser.cpp
)

add_subdirectory(applications)
add_subdirectory(signals-handlers)
add_subdirectory(tests)
