cmake_minimum_required(VERSION 3.13)

# The devices the networks compute on: the host one and the NVIDIA GPU one of
# the ENABLE_CUDA build (the cuda subdirectory).
add_library(
  DarknetCXXDevicesOBJ OBJECT
  HostNetworkDevice.cpp
  NetworkDevices.cpp
)

target_include_directories(
  DarknetCXXDevicesOBJ
  PRIVATE
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_BINARY_DIR}
    ${MOD_DARKNET_INCLUDES}
)

if(ENABLE_CUDA)
  add_subdirectory(cuda)
endif()
