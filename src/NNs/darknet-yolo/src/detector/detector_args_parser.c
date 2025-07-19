#include "src/detector/detector_args_parser.h"

#include <stdio.h>

#include <assert.h>

#include "src/detector/detector_context.h"
#include "src/utils.h"

int detector_args_parser(struct detector_context *ctx)
{
  assert(ctx != NULL);

  if (ctx == NULL) {
    fprintf(stderr, "detector_args_parser: invalid context provided");
    return 0;
  }

  int dont_show = find_arg(ctx->argc, ctx->argv, "-dont_show");
  int benchmark = find_arg(ctx->argc, ctx->argv, "-benchmark");
  int benchmark_layers = find_arg(ctx->argc, ctx->argv, "-benchmark_layers");
  // if (benchmark_layers) benchmark = 1;
  if (benchmark) dont_show = 1;
  int show = find_arg(ctx->argc, ctx->argv, "-show");
  int letter_box = find_arg(ctx->argc, ctx->argv, "-letter_box");
  int calc_map = find_arg(ctx->argc, ctx->argv, "-map");
  int map_points = find_int_arg(ctx->argc, ctx->argv, "-points", 0);
  int show_imgs = find_arg(ctx->argc, ctx->argv, "-show_imgs");
  int mjpeg_port = find_int_arg(ctx->argc, ctx->argv, "-mjpeg_port", -1);
  int avgframes = find_int_arg(ctx->argc, ctx->argv, "-avgframes", 3);
  int dontdraw_bbox = find_arg(ctx->argc, ctx->argv, "-dontdraw_bbox");
  int json_port = find_int_arg(ctx->argc, ctx->argv, "-json_port", -1);
  char *http_post_host =
      find_char_arg(ctx->argc, ctx->argv, "-http_post_host", 0);
  int time_limit_sec = find_int_arg(ctx->argc, ctx->argv, "-time_limit_sec", 0);
  char *out_filename = find_char_arg(ctx->argc, ctx->argv, "-out_filename", 0);
  char *json_file_output =
      find_char_arg(ctx->argc, ctx->argv, "-json_file_output", 0);
  char *outfile = find_char_arg(ctx->argc, ctx->argv, "-out", 0);
  char *prefix = find_char_arg(ctx->argc, ctx->argv, "-prefix", 0);
  float thresh = find_float_arg(ctx->argc, ctx->argv, "-thresh", .25);  // 0.24
  float iou_thresh =
      find_float_arg(ctx->argc, ctx->argv, "-iou_thresh", .5);  // 0.5 for mAP
  float hier_thresh = find_float_arg(ctx->argc, ctx->argv, "-hier", .5);
  int cam_index = find_int_arg(ctx->argc, ctx->argv, "-c", 0);
  int frame_skip = find_int_arg(ctx->argc, ctx->argv, "-s", 0);
  int num_of_clusters =
      find_int_arg(ctx->argc, ctx->argv, "-num_of_clusters", 5);
  int width = find_int_arg(ctx->argc, ctx->argv, "-width", -1);
  int height = find_int_arg(ctx->argc, ctx->argv, "-height", -1);
  // extended output in test mode (output of rect bound coords)
  // and for recall mode (extended output table-like format with results for
  // best_class fit)
  int ext_output = find_arg(ctx->argc, ctx->argv, "-ext_output");
  int save_labels = find_arg(ctx->argc, ctx->argv, "-save_labels");
  char *chart_path = find_char_arg(ctx->argc, ctx->argv, "-chart", 0);
  // While training, decide after how many epochs mAP will be calculated.
  // Default value is 4 which means the mAP will be calculated after each 4
  // epochs
  int mAP_epochs = find_int_arg(ctx->argc, ctx->argv, "-mAP_epochs", 4);
  if (ctx->argc < 4) {
    fprintf(stderr,
            "usage: %s %s [train/test/valid/demo/map] [data] [cfg] [weights "
            "(optional)]\n",
            ctx->argv[0], ctx->argv[1]);
    return 0;
  }
  char *gpu_list = find_char_arg(ctx->argc, ctx->argv, "-gpus", 0);
  int *gpus = 0;
  int gpu = 0;
  int ngpus = 0;
  if (gpu_list) {
    printf("%s\n", gpu_list);
    int len = (int)strlen(gpu_list);
    ngpus = 1;
    int i;
    for (i = 0; i < len; ++i) {
      if (gpu_list[i] == ',') ++ngpus;
    }
    gpus = (int *)xcalloc(ngpus, sizeof(int));
    for (i = 0; i < ngpus; ++i) {
      gpus[i] = atoi(gpu_list);
      gpu_list = strchr(gpu_list, ',') + 1;
    }
  } else {
    gpu = gpu_index;
    gpus = &gpu;
    ngpus = 1;
  }

  int clear = find_arg(ctx->argc, ctx->argv, "-clear");

  char *datacfg = ctx->argv[3];
  char *cfg = ctx->argv[4];
  char *weights = (ctx->argc > 5) ? ctx->argv[5] : 0;

  if (weights)
    if (strlen(weights) > 0)
      if (weights[strlen(weights) - 1] == 0x0d)
        weights[strlen(weights) - 1] = 0;

  char *filename = (ctx->argc > 6) ? ctx->argv[6] : 0;

  return 1;
}