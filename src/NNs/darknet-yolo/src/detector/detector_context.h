#ifndef DARKNET_DETECTOR_C_CONTEXT_STRUCT_H
#define DARKNET_DETECTOR_C_CONTEXT_STRUCT_H

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

  list *options;
  char *train_images;
  char *valid_images;
  char *backup_directory;
};

#ifdef __cplusplus
}
#endif

#endif  // DARKNET_DETECTOR_C_CONTEXT_STRUCT_H