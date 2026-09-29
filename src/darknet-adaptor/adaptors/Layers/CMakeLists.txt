cmake_minimum_required(VERSION 3.13)

# The memory of the darknet layers: the C++ ports of the make_*_layer() and
# resize_*_layer() functions, the computation stays in the darknet C code.
add_library(
  DarknetCXXLayersOBJ OBJECT
  ActivationLayer.cpp
  AvgpoolLayer.cpp
  BatchnormLayer.cpp
  ConnectedLayer.cpp
  ConvLstmLayer.cpp
  ConvolutionalLayer.cpp
  CostLayer.cpp
  CrnnLayer.cpp
  CropLayer.cpp
  DetectionLayer.cpp
  DropoutLayer.cpp
  EmptyLayer.cpp
  GaussianYoloLayer.cpp
  GruLayer.cpp
  ImplicitLayer.cpp
  LayersBuffers.cpp
  LocalLayer.cpp
  LstmLayer.cpp
  MaxpoolLayer.cpp
  NormalizationLayer.cpp
  RegionLayer.cpp
  ReorgLayer.cpp
  ReorgOldLayer.cpp
  RnnLayer.cpp
  RouteLayer.cpp
  SamLayer.cpp
  ScaleChannelsLayer.cpp
  ShortcutLayer.cpp
  SoftmaxLayer.cpp
  UpsampleLayer.cpp
  YoloLayer.cpp
)

target_include_directories(
  DarknetCXXLayersOBJ
  PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}
    ${MOD_DARKNET_INCLUDES}
)

target_link_libraries(
  ${PROJECT_BINARY_NAME}
  DarknetCXXLayersOBJ
)
