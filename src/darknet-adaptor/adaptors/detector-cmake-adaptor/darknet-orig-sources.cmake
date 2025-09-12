cmake_minimum_required(VERSION 3.13)

# tool from original root CMakeLists.txt file
set(Darknet_MAJOR_VERSION 0)
set(Darknet_MINOR_VERSION 2)
set(Darknet_PATCH_VERSION 5)
set(Darknet_BUILD_VERSION 4)

#set(
#  VERSION_STRING 
#  "${Darknet_MAJOR_VERSION}.${Darknet_MINOR_VERSION}.${Darknet_PATCH_VERSION}.${Darknet_BUILD_VERSION}"
#)

set(
  DARKNET_ORIG_C_SOURCES
  ${MOD_DARKNET_SRC_ROOT}/src/detector/subs/trainer/train_detector_ctx.c
  ${MOD_DARKNET_SRC_ROOT}/src/detector/subs/trainer/calc_map_init_ctx.c
  ${MOD_DARKNET_SRC_ROOT}/src/detector/subs/trainer/load_net_weights_ctx.c
  ${MOD_DARKNET_SRC_ROOT}/src/detector/subs/trainer/print_net_init_stats_ctx.c
  ${MOD_DARKNET_SRC_ROOT}/src/detector/subs/trainer/traverse_detection_layers_ctx.c
  ${MOD_DARKNET_SRC_ROOT}/src/detector/subs/trainer/prepare_load_args_ctx.c
  ${MOD_DARKNET_SRC_ROOT}/src/detector/subs/trainer/draw_train_chart_ctx.c
  ${MOD_DARKNET_SRC_ROOT}/src/detector/subs/trainer/train_detector_iteration_ctx.c
  ${MOD_DARKNET_SRC_ROOT}/src/detector/subs/trainer/sync_gpu_nets_ctx.c
  ${MOD_DARKNET_SRC_ROOT}/src/detector/subs/trainer/save_weights_final_ctx.c
  ${MOD_DARKNET_SRC_ROOT}/src/detector/subs/trainer/clear_context.c
  ${MOD_DARKNET_SRC_ROOT}/src/detector/subs/trainer/calc_map_iteration_ctx.c
  ${MOD_DARKNET_SRC_ROOT}/src/detector/subs/trainer/draw_train_loss_ctx.c
  ${MOD_DARKNET_SRC_ROOT}/src/detector/subs/trainer/init_pretrain_time_ctx.c
  ${MOD_DARKNET_SRC_ROOT}/src/detector/subs/trainer/pretrain_net_prepare_ctx.c
  ${MOD_DARKNET_SRC_ROOT}/src/detector/subs/trainer/print_net_stat_ctx.c
  ${MOD_DARKNET_SRC_ROOT}/src/detector/subs/trainer/recompute_avg_loss_ctx.c
  ${MOD_DARKNET_SRC_ROOT}/src/detector/subs/trainer/recompute_times_ctx.c
  ${MOD_DARKNET_SRC_ROOT}/src/detector/subs/trainer/save_truth_image.c
  ${MOD_DARKNET_SRC_ROOT}/src/detector/subs/trainer/save_weights_ctx.c
  ${MOD_DARKNET_SRC_ROOT}/src/detector/subs/trainer/train_network_ctx.c
  ${MOD_DARKNET_SRC_ROOT}/src/detector/subs/trainer/calc_map_free_ctx.c
  ${MOD_DARKNET_SRC_ROOT}/src/detector/subs/trainer/save_weights_best_ctx.c
  ${MOD_DARKNET_SRC_ROOT}/src/detector/subs/tester/save_detections_json_ctx.c
  ${MOD_DARKNET_SRC_ROOT}/src/detector/subs/tester/save_detections_labels_ctx.c
  ${MOD_DARKNET_SRC_ROOT}/src/detector/subs/tester/show_detections_ctx.c
  ${MOD_DARKNET_SRC_ROOT}/src/detector/subs/tester/test_detector_ctx.c
  ${MOD_DARKNET_SRC_ROOT}/src/detector/subs/tester/refine_image_filename_ctx.c
  ${MOD_DARKNET_SRC_ROOT}/src/detector/subs/tester/load_and_prepare_net.c
  ${MOD_DARKNET_SRC_ROOT}/src/darknet-fcns.c
  ${MOD_DARKNET_SRC_ROOT}/src/network.c
  ${MOD_DARKNET_SRC_ROOT}/src/data.c
  ${MOD_DARKNET_SRC_ROOT}/src/option_list.c
  ${MOD_DARKNET_SRC_ROOT}/src/utils.c
  ${MOD_DARKNET_SRC_ROOT}/src/parser.c
  ${MOD_DARKNET_SRC_ROOT}/src/list.c
  ${MOD_DARKNET_SRC_ROOT}/src/detector.c
  ${MOD_DARKNET_SRC_ROOT}/src/blas.c
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
  ${MOD_DARKNET_SRC_ROOT}/src/image.c
  ${MOD_DARKNET_SRC_ROOT}/src/activations.c
  ${MOD_DARKNET_SRC_ROOT}/src/layers/yolo_layer.c
  ${MOD_DARKNET_SRC_ROOT}/src/gemm.c
  ${MOD_DARKNET_SRC_ROOT}/src/layers/representation_layer.c
  ${MOD_DARKNET_SRC_ROOT}/src/im2col.c
  ${MOD_DARKNET_SRC_ROOT}/src/box.c
  ${MOD_DARKNET_SRC_ROOT}/src/tree.c
  ${MOD_DARKNET_SRC_ROOT}/src/matrix.c
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
  ${MOD_DARKNET_SRC_ROOT}/src/col2im.c
  ${MOD_DARKNET_SRC_ROOT}/src/randoms.c
  ${MOD_DARKNET_SRC_ROOT}/src/memory.c
  ${MOD_DARKNET_SRC_ROOT}/src/string_tools.c
  ${MOD_DARKNET_SRC_ROOT}/src/error.c
  ${MOD_DARKNET_SRC_ROOT}/src/layers/layers_tools.c
  ${MOD_DARKNET_SRC_ROOT}/src/clog/clog.c
  ${MOD_DARKNET_SRC_ROOT}/src/gpu_index_decls.c
)

set(
  DARKNET_ORIG_CXX_SOURCES
  ${MOD_DARKNET_SRC_ROOT}/src/image_opencv.cpp
)

configure_file(
  ${MOD_DARKNET_SRC_ROOT}/src/version.h.in 
  ${CMAKE_BINARY_DIR}/version.h
)

