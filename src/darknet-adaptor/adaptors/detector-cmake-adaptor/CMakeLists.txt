cmake_minimum_required(VERSION 3.13)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")

include(darknet-orig-sources)

if (DARKNET_ORIG_C_SOURCES STREQUAL "")
  message(FATAL_ERROR "No DARKNET_ORIG_C_SOURCES variable value found")
endif()

if (DARKNET_ORIG_CXX_SOURCES STREQUAL "")
  message(FATAL_ERROR "No DARKNET_ORIG_CXX_SOURCES variable value found")
endif()

add_library(
  DarknetCXXADetectorTrainerOrigDarknetCMakeCAdaptorOBJ OBJECT
  ${DARKNET_ORIG_C_SOURCES}
)

add_library(
  DarknetCXXADetectorTrainerOrigDarknetCMakeCXXAdaptorOBJ OBJECT
  ${DARKNET_ORIG_CXX_SOURCES}
)

target_link_options(
  DarknetCXXADetectorTrainerOrigDarknetCMakeCXXAdaptorOBJ
  PRIVATE ${EXTRA_C_LINK_OPTIONS}
)

target_include_directories(
  DarknetCXXADetectorTrainerOrigDarknetCMakeCAdaptorOBJ 
  PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}
    ${MOD_DARKNET_INCLUDES}
    ${OpenCV_INCLUDE_DIRS}
)

target_include_directories(
  DarknetCXXADetectorTrainerOrigDarknetCMakeCXXAdaptorOBJ 
  PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}
    ${MOD_DARKNET_INCLUDES}
    ${OpenCV_INCLUDE_DIRS}
)

set_target_properties(
  DarknetCXXADetectorTrainerOrigDarknetCMakeCAdaptorOBJ 
  PROPERTIES 
  COMPILE_OPTIONS ""
)

target_compile_options(
  DarknetCXXADetectorTrainerOrigDarknetCMakeCAdaptorOBJ 
  PRIVATE 
  ${EXTRA_C_COMPILE_OPTIONS}
  ${SANITIZERS_LIST}
)

target_compile_options(
  DarknetCXXADetectorTrainerOrigDarknetCMakeCXXAdaptorOBJ 
  PRIVATE 
  ${SANITIZERS_LIST}
)

target_link_options(
  DarknetCXXADetectorTrainerOrigDarknetCMakeCAdaptorOBJ
  PRIVATE ${SANITIZERS_LIST_LINK}
)

target_link_options(
  DarknetCXXADetectorTrainerOrigDarknetCMakeCXXAdaptorOBJ
  PRIVATE ${SANITIZERS_LIST_LINK}
)

target_link_libraries(
  ${PROJECT_BINARY_NAME}
  DarknetCXXADetectorTrainerOrigDarknetCMakeCAdaptorOBJ
  DarknetCXXADetectorTrainerOrigDarknetCMakeCXXAdaptorOBJ
)
