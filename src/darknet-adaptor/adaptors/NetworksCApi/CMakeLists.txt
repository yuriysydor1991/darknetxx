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
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}
    ${MOD_DARKNET_INCLUDES}
)

target_link_libraries(
  ${PROJECT_BINARY_NAME}
  DarknetCXXNetworksCApiOBJ
)

add_subdirectory(tests)
