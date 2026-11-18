cmake_minimum_required(VERSION 3.13)

# The GPU memory of the networks: the device copy of the layers memory the
# adaptors keep, the way the original darknet allocated it.
add_library(
  DarknetCXXCudaDevicesOBJ OBJECT
  CudaArrays.cpp
  CudaNetworkDevice.cpp
  CudaNetworkInputs.cpp
  CudnnDescriptors.cpp
  layers/CudaBatchnormLayer.cpp
  layers/CudaConnectedLayer.cpp
  layers/CudaContrastiveLayer.cpp
  layers/CudaConvLstmLayer.cpp
  layers/CudaConvolutionalLayer.cpp
  layers/CudaCropLayer.cpp
  layers/CudaDropoutLayer.cpp
  layers/CudaEmptyLayer.cpp
  layers/CudaGruLayer.cpp
  layers/CudaImplicitLayer.cpp
  layers/CudaLayer.cpp
  layers/CudaLayers.cpp
  layers/CudaLocalLayer.cpp
  layers/CudaLstmLayer.cpp
  layers/CudaMaxpoolLayer.cpp
  layers/CudaOutputLayer.cpp
  layers/CudaRnnLayer.cpp
  layers/CudaShortcutLayer.cpp
)

target_include_directories(
  DarknetCXXCudaDevicesOBJ
  PRIVATE
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_BINARY_DIR}
    ${MOD_DARKNET_INCLUDES}
)

add_subdirectory(tests)
