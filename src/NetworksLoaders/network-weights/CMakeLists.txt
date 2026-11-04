cmake_minimum_required(VERSION 3.13)

# The darknet networks of the darknetxx-datadrivers library: the view of the
# library over the layers and the arrays of the darknet network struct (the
# walk of the original save_weights_upto() and load_weights_upto()), which the
# readers of the library load the weights files into and the writers save the
# files of, and the converter of the files by the darknet network of the cfg.
add_library(
  DarknetCXXNetworkWeightsOBJ OBJECT
  NetworkWeights.cpp
  NetworkWeightsConverter.cpp
)

target_include_directories(
  DarknetCXXNetworkWeightsOBJ
  PRIVATE
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_BINARY_DIR}
    ${MOD_DARKNET_INCLUDES}
)

add_subdirectory(tests)
