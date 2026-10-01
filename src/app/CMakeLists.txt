cmake_minimum_required(VERSION 3.13)

# Exposed to the whole src/app component, so every subcomponent test reaches
# the mocks of the neighbouring ones.
set(APP_MOCKS_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/tests/mocks")
set(SIGNALS_HANDLERS_MOCKS_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/signals-handlers/tests/mocks")
set(POSIX_SIGNALS_HANDLERS_MOCKS_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/signals-handlers/handlers/posix/tests/mocks")

# The sources both application binaries are built of: the main one and the
# network weights files converter only one (see the
# src/darknetxxConverter-target.cmake), which compiles them with the
# DARKNETXX_CONVERTER_ONLY definition.
set(
  APP_SHARED_SOURCES
  ${CMAKE_CURRENT_SOURCE_DIR}/ApplicationFactory.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/ApplicationHelpPrinter.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/CommandLineParser.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/ApplicationVersionPrinter.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/converter/ApplicationConverterComponent.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/converter/ApplicationFormatsConverter.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/converter/ConversionFormats.cpp
)

# The app::Application class, which starts the training and the detection, is
# the main binary one alone.
target_sources(
  ${PROJECT_BINARY_NAME}
  PRIVATE ApplicationContext.cpp
  PRIVATE Application.cpp
  PRIVATE ${APP_SHARED_SOURCES}
)

# The converter gets the ApplicationContext of the networks core it links.
if(TARGET ${PROJECT_CONVERTER_BINARY_NAME})
  target_sources(
    ${PROJECT_CONVERTER_BINARY_NAME}
    PRIVATE ${APP_SHARED_SOURCES}
  )
endif()

add_subdirectory(signals-handlers)
add_subdirectory(tests)
