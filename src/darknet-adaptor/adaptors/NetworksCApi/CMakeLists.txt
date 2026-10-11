cmake_minimum_required(VERSION 3.13)

# The darknet C API of the network memory and the weights files
# (parse_network_cfg(), load_weights(), resize_network(), free_network() and
# the like) the darknet C code calls, implemented on top of the C++ adaptors.
add_library(
  DarknetCXXNetworksCApiOBJ OBJECT
  NetworksCApi.cpp
  NetworksRegistry.cpp
)

target_include_directories(
  DarknetCXXNetworksCApiOBJ 
  PRIVATE
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_BINARY_DIR}
    ${MOD_DARKNET_INCLUDES}
)

add_subdirectory(tests)
