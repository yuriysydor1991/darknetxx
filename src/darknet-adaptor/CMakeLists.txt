cmake_minimum_required(VERSION 3.13)

# Only the OpenCV modules the Darknet code calls, so the binary (and the
# packages made of it) doesn't depend on every installed OpenCV module.
find_package(
  OpenCV REQUIRED
  COMPONENTS core imgproc imgcodecs highgui videoio
)

# The DarknetContext stop watcher runs in an std::thread one, which needs the
# system threads library on the platforms that keep it apart from the libc.
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

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
)

target_include_directories(
  DarknetCXXModDarknetAdaptorOBJ 
  PUBLIC
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}
    ${MOD_DARKNET_INCLUDES}
    ${OpenCV_INCLUDE_DIRS}
)

# Apart from the controller, so the tests of the network infrastructure link
# the context it loads the networks into without the application.
add_library(
  DarknetCXXDarknetContextOBJ OBJECT
  DarknetContext.cpp
)

target_include_directories(
  DarknetCXXDarknetContextOBJ 
  PUBLIC
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}
    ${MOD_DARKNET_INCLUDES}
    ${OpenCV_INCLUDE_DIRS}
)

# The darknet C computation with the C++ network infrastructure (the layers
# and networks memory, the loaders, the weights files, the darknet C API of
# them), the whole darknet core without the application: the tests link it.
add_library(DarknetXXNetworksCore INTERFACE)

target_sources(
  DarknetXXNetworksCore
  INTERFACE
    $<TARGET_OBJECTS:DarknetCXXADetectorTrainerOrigDarknetCMakeCAdaptorOBJ>
    $<TARGET_OBJECTS:DarknetCXXADetectorTrainerOrigDarknetCMakeCXXAdaptorOBJ>
    $<TARGET_OBJECTS:DarknetCXXAAdaptorsGeneralOBJ>
    $<TARGET_OBJECTS:DarknetCXXALayerAdaptorOBJ>
    $<TARGET_OBJECTS:DarknetCXXANetworkAdaptorOBJ>
    $<TARGET_OBJECTS:DarknetCXXLayersOBJ>
    $<TARGET_OBJECTS:DarknetCXXNetworkLoaderOrigDefaultOBJ>
    $<TARGET_OBJECTS:DarknetCXXWeightsModelOBJ>
    $<TARGET_OBJECTS:DarknetCXXOrigWeightsOBJ>
    $<TARGET_OBJECTS:DarknetCXXDxxwjz1V1OBJ>
    $<TARGET_OBJECTS:DarknetCXXWeightsConverterOBJ>
    $<TARGET_OBJECTS:DarknetCXXNetworksCApiOBJ>
    $<TARGET_OBJECTS:DarknetCXXDarknetContextOBJ>
    $<TARGET_OBJECTS:DarknetCXXHelpersComponentOBJ>
    $<TARGET_OBJECTS:DarknetXXLog4CInterfaceObj>
    $<TARGET_OBJECTS:TemplateProjectDefaultLoggerObj>
    $<TARGET_OBJECTS:TemplateProjectZlibControllerObj>
    # the application context the DarknetContext is made of
    ${CMAKE_SOURCE_DIR}/src/app/ApplicationContext.cpp
)

target_include_directories(
  DarknetXXNetworksCore
  INTERFACE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}
    ${MOD_DARKNET_INCLUDES}
    ${OpenCV_INCLUDE_DIRS}
)

target_compile_definitions(
  DarknetXXNetworksCore
  INTERFACE ${OPENCV_DECL}
)

target_link_libraries(
  DarknetXXNetworksCore
  INTERFACE
    ${OpenCV_LIBS}
    Threads::Threads
    nlohmann_json::nlohmann_json
    ZLIB::ZLIB
)

target_compile_definitions(
  ${PROJECT_BINARY_NAME}
  PUBLIC ${OPENCV_DECL}
)

target_link_libraries(
  ${PROJECT_BINARY_NAME}
  DarknetCXXModDarknetAdaptorOBJ
  DarknetCXXDarknetContextOBJ
  ${OpenCV_LIBS}
  Threads::Threads
)

