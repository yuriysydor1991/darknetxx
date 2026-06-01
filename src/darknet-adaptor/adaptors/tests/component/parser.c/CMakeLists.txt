cmake_minimum_required(VERSION 3.13)

configure_file(
  CTEST_darknet_parser_provider.h.in
  CTEST_darknet_parser_provider.h
)

add_executable(
  CTEST_darknet_parser
  CTEST_darknet_parser.cpp
  CTEST_darknet_parser_provider_get_1.cpp
  CTEST_darknet_parser_provider_get_2.cpp
  NetworkTracer.cpp
  ${MOD_DARKNET_SRC_ROOT}/src/memory.c
  ${MOD_DARKNET_SRC_ROOT}/src/string_tools.c
  ${MOD_DARKNET_SRC_ROOT}/src/error.c
  ${MOD_DARKNET_SRC_ROOT}/src/parser.c
  ${MOD_DARKNET_SRC_ROOT}/src/clog/clog.c
  ${MOD_DARKNET_SRC_ROOT}/src/layers/connected_layer.c
  ${MOD_DARKNET_SRC_ROOT}/src/layers/conv_lstm_layer.c
  ${MOD_DARKNET_SRC_ROOT}/src/layers/convolutional_layer.c
  ${MOD_DARKNET_SRC_ROOT}/src/layers/cost_layer.c
  ${MOD_DARKNET_SRC_ROOT}/src/layers/crnn_layer.c
  ${MOD_DARKNET_SRC_ROOT}/src/layers/crop_layer.c
  ${MOD_DARKNET_SRC_ROOT}/src/layers/batchnorm_layer.c
  ${MOD_DARKNET_SRC_ROOT}/src/layers/avgpool_layer.c
  ${MOD_DARKNET_SRC_ROOT}/src/layers/activation_layer.c
  ${MOD_DARKNET_SRC_ROOT}/src/layers/detection_layer.c
  ${MOD_DARKNET_SRC_ROOT}/src/layers/local_layer.c
  ${MOD_DARKNET_SRC_ROOT}/src/layers/route_layer.c
  ${MOD_DARKNET_SRC_ROOT}/src/layers/gaussian_yolo_layer.c
  ${MOD_DARKNET_SRC_ROOT}/src/layers/maxpool_layer.c
  ${MOD_DARKNET_SRC_ROOT}/src/layers/normalization_layer.c
  ${MOD_DARKNET_SRC_ROOT}/src/layers/region_layer.c
  ${MOD_DARKNET_SRC_ROOT}/src/layers/layer.c
  ${MOD_DARKNET_SRC_ROOT}/src/layers/scale_channels_layer.c
  ${MOD_DARKNET_SRC_ROOT}/src/layers/dropout_layer.c
  ${MOD_DARKNET_SRC_ROOT}/src/layers/upsample_layer.c
  ${MOD_DARKNET_SRC_ROOT}/src/layers/shortcut_layer.c
  ${MOD_DARKNET_SRC_ROOT}/src/layers/reorg_layer.c
  ${MOD_DARKNET_SRC_ROOT}/src/layers/softmax_layer.c
  ${MOD_DARKNET_SRC_ROOT}/src/layers/lstm_layer.c
  ${MOD_DARKNET_SRC_ROOT}/src/layers/gru_layer.c
  ${MOD_DARKNET_SRC_ROOT}/src/layers/rnn_layer.c
  ${MOD_DARKNET_SRC_ROOT}/src/layers/sam_layer.c
  ${MOD_DARKNET_SRC_ROOT}/src/layers/reorg_old_layer.c
  ${MOD_DARKNET_SRC_ROOT}/src/layers/representation_layer.c
  ${MOD_DARKNET_SRC_ROOT}/src/layers/yolo_layer.c
  ${MOD_DARKNET_SRC_ROOT}/src/blas.c
  ${MOD_DARKNET_SRC_ROOT}/src/activations.c
  ${MOD_DARKNET_SRC_ROOT}/src/utils.c
  ${MOD_DARKNET_SRC_ROOT}/src/network.c
  ${MOD_DARKNET_SRC_ROOT}/src/randoms.c
  ${MOD_DARKNET_SRC_ROOT}/src/memory.c
  ${MOD_DARKNET_SRC_ROOT}/src/string_tools.c
  ${MOD_DARKNET_SRC_ROOT}/src/error.c
  ${MOD_DARKNET_SRC_ROOT}/src/layers/layers_tools.c
  ${MOD_DARKNET_SRC_ROOT}/src/list.c
  ${MOD_DARKNET_SRC_ROOT}/src/image.c
  ${MOD_DARKNET_SRC_ROOT}/src/matrix.c
  ${MOD_DARKNET_SRC_ROOT}/src/box.c
  ${MOD_DARKNET_SRC_ROOT}/src/tree.c
  ${MOD_DARKNET_SRC_ROOT}/src/gemm.c
  ${MOD_DARKNET_SRC_ROOT}/src/option_list.c
  ${MOD_DARKNET_SRC_ROOT}/src/im2col.c
  ${MOD_DARKNET_SRC_ROOT}/src/image_opencv.cpp
  ${MOD_DARKNET_SRC_ROOT}/src/col2im.c
  ${MOD_DARKNET_SRC_ROOT}/src/data.c
  ${MOD_DARKNET_SRC_ROOT}/src/gpu_index_decls.c
)

target_compile_definitions(
  CTEST_darknet_parser
  PUBLIC ${OPENCV_DECL}
)

target_include_directories(
  CTEST_darknet_parser
  PRIVATE 
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_BINARY_DIR}
    ${MOD_DARKNET_INCLUDES}
    ${CMAKE_CURRENT_BINARY_DIR}
    ${OpenCV_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(
  CTEST_darknet_parser
  ${OpenCV_LIBS}
  GTest::gtest_main GTest::gmock 
  DarknetXXLog4CInterfaceObj 
  TemplateProjectDefaultLoggerObj
)

include(GoogleTest)

gtest_add_tests(
  TARGET CTEST_darknet_parser
  TEST_SUFFIX .noArgs
  TEST_LIST noArgsTests
)

set_tests_properties(${noArgsTests} PROPERTIES TIMEOUT 600)

