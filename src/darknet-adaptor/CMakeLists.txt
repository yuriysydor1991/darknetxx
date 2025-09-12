cmake_minimum_required(VERSION 3.13)

find_package(OpenCV REQUIRED)

message(STATUS "OpenCV includes: ${OpenCV_INCLUDE_DIRS}")
message(STATUS "OpenCV libs: ${OpenCV_LIBS}")

set(OPENCV_DECL "OPENCV=1")
add_compile_definitions(${OPENCV_DECL})

set(
  MOD_DARKNET_SRC_ROOT
  "${CMAKE_CURRENT_SOURCE_DIR}/adaptors/detector-cmake-adaptor/darknet"
)

set(
  MOD_DARKNET_INCLUDES
  ${MOD_DARKNET_SRC_ROOT}
  ${MOD_DARKNET_SRC_ROOT}/src
  ${MOD_DARKNET_SRC_ROOT}/3rdparty/stb/include
)

add_subdirectory(adaptors)

add_library(
  DarknetCXXModDarknetAdaptorOBJ OBJECT
  DarknetController.cpp
  DarknetContext.cpp
)

target_include_directories(
  DarknetCXXModDarknetAdaptorOBJ 
  PUBLIC
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}
    ${MOD_DARKNET_INCLUDES}
    ${OpenCV_INCLUDE_DIRS}
)

target_compile_definitions(
  ${PROJECT_BINARY_NAME}
  PUBLIC ${OPENCV_DECL}
)

target_link_libraries(
  ${PROJECT_BINARY_NAME}
  DarknetCXXModDarknetAdaptorOBJ
  ${OpenCV_LIBS}
)

