cmake_minimum_required(VERSION 3.13)

target_sources(
  ${PROJECT_BINARY_NAME}
  PRIVATE ContinueSignalsHandler.cpp
  PRIVATE IgnoredSignalsHandler.cpp
  PRIVATE PauseSignalsHandler.cpp
  PRIVATE PosixSignalsHandler.cpp
  PRIVATE ReloadSignalsHandler.cpp
  PRIVATE StopSignalsHandler.cpp
  PRIVATE UserSignalsHandler.cpp
)

# the network weights files converter only binary handles the OS signals the
# same way
if(TARGET ${PROJECT_CONVERTER_BINARY_NAME})
  target_sources(
    ${PROJECT_CONVERTER_BINARY_NAME}
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/ContinueSignalsHandler.cpp
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/IgnoredSignalsHandler.cpp
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/PauseSignalsHandler.cpp
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/PosixSignalsHandler.cpp
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/ReloadSignalsHandler.cpp
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/StopSignalsHandler.cpp
    PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/UserSignalsHandler.cpp
  )
endif()

add_subdirectory(tests)
