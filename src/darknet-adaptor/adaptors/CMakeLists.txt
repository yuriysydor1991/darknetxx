cmake_minimum_required(VERSION 3.13)

# the src/detector-cmake-adaptor component, the original darknet code built
# with the darknet includes and the OpenCV definitions of the adaptors
add_subdirectory(
  ${PROJECT_SOURCE_DIR}/src/detector-cmake-adaptor
  ${PROJECT_BINARY_DIR}/src/detector-cmake-adaptor
)

add_library(
  DarknetCXXAAdaptorsGeneralOBJ OBJECT
  StructAdaptorHelper.cpp
)

target_include_directories(
  DarknetCXXAAdaptorsGeneralOBJ 
  PRIVATE
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_BINARY_DIR}
    ${MOD_DARKNET_INCLUDES}
)

add_subdirectory(ADetectorTrainer)
add_subdirectory(ADetector)
add_subdirectory(ALayerAdaptor)
add_subdirectory(ANetworkAdaptor)
add_subdirectory(Layers)

# the src/NetworksLoaders component, built with the darknet includes and the
# OpenCV definitions of the adaptors
add_subdirectory(
  ${PROJECT_SOURCE_DIR}/src/NetworksLoaders
  ${PROJECT_BINARY_DIR}/src/NetworksLoaders
)

add_subdirectory(NetworksCApi)
add_subdirectory(TrainingData)
add_subdirectory(TrainingJournal)
add_subdirectory(TrainingChart)
add_subdirectory(tests)
