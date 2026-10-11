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

# The backtrace() of the Darknet error log, a part of the glibc and the
# libexecinfo library of the FreeBSD.
find_package(Backtrace REQUIRED)

message(STATUS "OpenCV includes: ${OpenCV_INCLUDE_DIRS}")
message(STATUS "OpenCV libs: ${OpenCV_LIBS}")

# The USE_CMAKE_LIBS leaves out the MSVC #pragma comment(lib) of the Darknet
# code, which the Clang keeps in the objects for the lld to look the
# opencv_world*.lib up, while the CMake links the OpenCV itself.
set(OPENCV_DECL "OPENCV=1" "USE_CMAKE_LIBS=1")
add_compile_definitions(${OPENCV_DECL})

set(
  MOD_DARKNET_SRC_ROOT
  "${PROJECT_SOURCE_DIR}/src/detector-cmake-adaptor/darknet"
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
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_BINARY_DIR}
    ${MOD_DARKNET_INCLUDES}
    ${OpenCV_INCLUDE_DIRS}
)

# the darknet headers of the controller declare the OpenCV types under it
target_compile_definitions(
  DarknetCXXModDarknetAdaptorOBJ
  PUBLIC ${OPENCV_DECL}
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
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_BINARY_DIR}
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
    $<TARGET_OBJECTS:DarknetCXXDxxwjz2V2OBJ>
    $<TARGET_OBJECTS:DarknetCXXDxxwjz2pV1OBJ>
    $<TARGET_OBJECTS:DarknetCXXDxxwjz3V3OBJ>
    $<TARGET_OBJECTS:DarknetCXXDxxwjz3pV1OBJ>
    $<TARGET_OBJECTS:DarknetCXXYolo4FolderOBJ>
    $<TARGET_OBJECTS:DarknetCXXWeightsFormatsOBJ>
    $<TARGET_OBJECTS:DarknetCXXWeightsConverterOBJ>
    $<TARGET_OBJECTS:DarknetCXXNetworksCApiOBJ>
    $<TARGET_OBJECTS:DarknetCXXTrainingDataOBJ>
    $<TARGET_OBJECTS:DarknetCXXTrainingJournalOBJ>
    $<TARGET_OBJECTS:DarknetCXXTrainingChartOBJ>
    $<TARGET_OBJECTS:DarknetCXXDarknetContextOBJ>
    $<TARGET_OBJECTS:DarknetXXLibTypesOBJ>
    $<TARGET_OBJECTS:DarknetCXXHelpersComponentOBJ>
    $<TARGET_OBJECTS:DarknetXXLog4CInterfaceObj>
    $<TARGET_OBJECTS:DarknetXXDefaultLoggerObj>
    $<TARGET_OBJECTS:DarknetXXZlibControllerObj>
)

target_include_directories(
  DarknetXXNetworksCore
  INTERFACE
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_BINARY_DIR}
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
    ${Backtrace_LIBRARIES}
)

