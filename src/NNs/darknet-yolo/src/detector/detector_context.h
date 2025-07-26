#ifndef DARKNET_DETECTOR_C_CONTEXT_STRUCT_H
#define DARKNET_DETECTOR_C_CONTEXT_STRUCT_H

#include "src/list.h"
#include "darknet.h"
#include "image.h"

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

  // from train_detector_ctx function

  list *options;
  char *train_images;
  char *valid_images;
  char *backup_directory;

  char *base;

  char windows_name[100];
  mat_cv *img;
  float max_img_loss;
  int number_of_lines;
  int img_size;
  int draw_precision;

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

  int next_map_calc;

  load_args args;
};

#ifdef __cplusplus
}
#endif

#endif  // DARKNET_DETECTOR_C_CONTEXT_STRUCT_H