cmake_minimum_required(VERSION 3.13)

# Exposed to the whole src/app component, so every subcomponent test reaches
# the mocks of the neighbouring ones.
set(APP_MOCKS_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/tests/mocks")
set(SIGNALS_HANDLERS_MOCKS_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/signals-handlers/tests/mocks")
set(POSIX_SIGNALS_HANDLERS_MOCKS_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/signals-handlers/handlers/posix/tests/mocks")

target_sources(
  ${PROJECT_BINARY_NAME}
  PRIVATE ApplicationContext.cpp
  PRIVATE Application.cpp
  PRIVATE ApplicationFactory.cpp
  PRIVATE ApplicationHelpPrinter.cpp
  PRIVATE CommandLineParser.cpp
  PRIVATE ApplicationVersionPrinter.cpp
)

add_subdirectory(signals-handlers)
add_subdirectory(tests)
