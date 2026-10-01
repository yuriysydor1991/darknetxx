cmake_minimum_required(VERSION 3.13)

target_sources(
  ${PROJECT_BINARY_NAME}
  PRIVATE ASignalsHandler.cpp
)

# the network weights files converter only binary handles the OS signals the
# same way
if(TARGET ${PROJECT_CONVERTER_BINARY_NAME})
  target_sources(
    ${PROJECT_CONVERTER_BINARY_NAME}
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/ASignalsHandler.cpp
  )
endif()

add_subdirectory(posix)
add_subdirectory(tests)
