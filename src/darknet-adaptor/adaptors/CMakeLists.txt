cmake_minimum_required(VERSION 3.13)

add_subdirectory(detector-cmake-adaptor)

add_library(
  DarknetCXXAAdaptorsGeneralOBJ OBJECT
  StructAdaptorHelper.cpp
)

target_include_directories(
  DarknetCXXAAdaptorsGeneralOBJ 
  PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}
    ${MOD_DARKNET_INCLUDES}
)

target_link_libraries(
  ${PROJECT_BINARY_NAME}
  DarknetCXXAAdaptorsGeneralOBJ
)

add_subdirectory(ADetectorTrainer)
add_subdirectory(ADetector)
add_subdirectory(tests)
#add_subdirectory(NetworksLoaders)
#add_subdirectory(ALayerAdaptor)
#add_subdirectory(ANetworkAdaptor)

