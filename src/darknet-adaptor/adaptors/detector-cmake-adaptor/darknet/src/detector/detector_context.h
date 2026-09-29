#ifndef DARKNET_DETECTOR_C_CONTEXT_STRUCT_H
#define DARKNET_DETECTOR_C_CONTEXT_STRUCT_H

#include <limits.h>

#include "darknet.h"
#include "image.h"
#include "image_opencv.h"
#include "src/list.h"

#ifdef __cplusplus
extern "C" {
#endif

struct detector_context
{
  int argc;
  char **argv;

  int dont_show;
  int benchmark;
  int benchmark_layers;
  int show;
  int letter_box;
  int calc_map;
  int map_points;
  int show_imgs;
  int mjpeg_port;
  int avgframes;
  int dontdraw_bbox;
  int json_port;
  char *http_post_host;
  int time_limit_sec;
  char *filename;
  char *out_filename;
  char *json_file_output;
  char *outfile;
  char *prefix;
  float thresh;
  float iou_thresh;
  float hier_thresh;
  int cam_index;
  int frame_skip;
  int num_of_clusters;
  int width;
  int height;
  // extended output in test mode (output of rect bound coords)
  // and for recall mode (extended output table-like format with results for
  // best_class fit)
  int ext_output;
  int save_labels;
  char *chart_path;
  // While training, decide after how many epochs mAP will be calculated.
  // Default value is 4 which means the mAP will be calculated after each 4
  // epochs
  int mAP_epochs;
  int clear;
  char *datacfg;
  char *cfg;
  char *weights;

  char *gpu_list;
  int *gpus;
  int gpu;
  int ngpus;

  int save_chart_every_iter;

  // from train_detector_ctx function

  char *base;

  char windows_name[PATH_MAX];
  mat_cv *img;
  float max_img_loss;
  int number_of_lines;
  int img_size;
  // the --draw-precision flag: the mAP curve is drawn on every iteration
  int draw_precision;
  // the mAP is calculated on the current iteration
  int map_calculated;

  int init_w;
  int init_h;
  int init_b;

  int actual_batch_size;

  network net_map;

  network *nets;

  data train;
  data buffer;

  network net;

  char **paths;
  int imgs;
  list *plist;
  int classes;
  int train_images_num;
  layer l;

  int iter_save, iter_save_last, iter_map;

  float mean_average_precision;
  float best_map;
  float loss;
  float avg_loss;
  float avg_contrastive_acc;

  double time, time_remaining, avg_time, alpha_time;
  double load_time;

  int next_map_calc;

  int current_iteration;

  int count;

  pthread_t load_thread;

  load_args args;

  // from the test_detector_ctx
  detection *dets;
  int nboxes;

  image im;
  image sized;

  char *input;
  double predict_time_ms;

  list *options;

  int names_size;
  char **names;  // loaded classes names

  image **alphabet;  // the alphabet images to display dets

  // the class names file given on the command line, NULL to use the names=
  // key of the network cfg [net] section
  char *name_list;

  // customs
  int dont_resize_network;

  // do reload images every iteration
  int reload_data;

  // a background load of the next training batch into the buffer is running
  int data_loading;

  // the next iteration must load a fresh batch (the network input changed)
  int data_reload_forced;

  /// @brief A non zero value asks the computation to stop. Another thread
  /// raises it, so access it with the detector_context_is_stopped and the
  /// detector_context_set_stop functions only.
  volatile int stop;

  unsigned int resize_network_every_nth;

  unsigned int nth_iteration_data_reload;

  // stop training when less than specified value is reached
  double stop_less_avg_loss;
};

/// @brief Tells if the computation is asked to stop. Thread safe.
static inline int detector_context_is_stopped(
    const struct detector_context *ctx)
{
#if defined(__GNUC__) || defined(__clang__)
  return __atomic_load_n(&ctx->stop, __ATOMIC_ACQUIRE) != 0;
#else
  return ctx->stop != 0;
#endif
}

/// @brief Asks the computation to stop with a non zero value. Thread safe.
static inline void detector_context_set_stop(struct detector_context *ctx,
                                             const int stop)
{
#if defined(__GNUC__) || defined(__clang__)
  __atomic_store_n(&ctx->stop, stop, __ATOMIC_RELEASE);
#else
  ctx->stop = stop;
#endif
}

#ifdef __cplusplus
}
#endif

#endif  // DARKNET_DETECTOR_C_CONTEXT_STRUCT_H
