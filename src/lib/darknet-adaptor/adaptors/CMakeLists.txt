cmake_minimum_required(VERSION 3.13)

add_subdirectory(detector-cmake-adaptor)

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
add_subdirectory(NetworksLoaders)
add_subdirectory(NetworksCApi)
add_subdirectory(TrainingJournal)
add_subdirectory(TrainingChart)
add_subdirectory(tests)
