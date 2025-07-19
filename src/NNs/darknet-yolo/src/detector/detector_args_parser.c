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

  ctx->dont_show = find_arg(ctx->argc, ctx->argv, "-dont_show");
  ctx-> benchmark = find_arg(ctx->argc, ctx->argv, "-benchmark");
  ctx-> benchmark_layers = find_arg(ctx->argc, ctx->argv, "-benchmark_layers");
  // if (benchmark_layers) benchmark = 1;
  if (ctx->benchmark) ctx->dont_show = 1;
  ctx-> show = find_arg(ctx->argc, ctx->argv, "-show");
  ctx-> letter_box = find_arg(ctx->argc, ctx->argv, "-letter_box");
  ctx-> calc_map = find_arg(ctx->argc, ctx->argv, "-map");
  ctx-> map_points = find_int_arg(ctx->argc, ctx->argv, "-points", 0);
  ctx-> show_imgs = find_arg(ctx->argc, ctx->argv, "-show_imgs");
  ctx-> mjpeg_port = find_int_arg(ctx->argc, ctx->argv, "-mjpeg_port", -1);
  ctx-> avgframes = find_int_arg(ctx->argc, ctx->argv, "-avgframes", 3);
  ctx-> dontdraw_bbox = find_arg(ctx->argc, ctx->argv, "-dontdraw_bbox");
  ctx-> json_port = find_int_arg(ctx->argc, ctx->argv, "-json_port", -1);
  ctx->http_post_host =
      find_char_arg(ctx->argc, ctx->argv, "-http_post_host", 0);
  ctx-> time_limit_sec = find_int_arg(ctx->argc, ctx->argv, "-time_limit_sec", 0);
  ctx->out_filename = find_char_arg(ctx->argc, ctx->argv, "-out_filename", 0);
  ctx->json_file_output =
      find_char_arg(ctx->argc, ctx->argv, "-json_file_output", 0);
  ctx->outfile = find_char_arg(ctx->argc, ctx->argv, "-out", 0);
  ctx->prefix = find_char_arg(ctx->argc, ctx->argv, "-prefix", 0);
  ctx-> thresh = find_float_arg(ctx->argc, ctx->argv, "-thresh", .25);  // 0.24
  ctx-> iou_thresh =
      find_float_arg(ctx->argc, ctx->argv, "-iou_thresh", .5);  // 0.5 for mAP
  ctx-> hier_thresh = find_float_arg(ctx->argc, ctx->argv, "-hier", .5);
  ctx-> cam_index = find_int_arg(ctx->argc, ctx->argv, "-c", 0);
  ctx-> frame_skip = find_int_arg(ctx->argc, ctx->argv, "-s", 0);
  ctx-> num_of_clusters =
      find_int_arg(ctx->argc, ctx->argv, "-num_of_clusters", 5);
  ctx-> width = find_int_arg(ctx->argc, ctx->argv, "-width", -1);
  ctx-> height = find_int_arg(ctx->argc, ctx->argv, "-height", -1);
  // extended output in test mode (output of rect bound coords)
  // and for recall mode (extended output table-like format with results for
  // best_class fit)
  ctx-> ext_output = find_arg(ctx->argc, ctx->argv, "-ext_output");
  ctx-> save_labels = find_arg(ctx->argc, ctx->argv, "-save_labels");
  ctx->chart_path = find_char_arg(ctx->argc, ctx->argv, "-chart", 0);
  // While training, decide after how many epochs mAP will be calculated.
  // Default value is 4 which means the mAP will be calculated after each 4
  // epochs
  ctx-> mAP_epochs = find_int_arg(ctx->argc, ctx->argv, "-mAP_epochs", 4);
  if (ctx->argc < 4) {
    fprintf(stderr,
            "usage: %s %s [train/test/valid/demo/map] [data] [cfg] [weights "
            "(optional)]\n",
            ctx->argv[0], ctx->argv[1]);
    return 0;
  }
  ctx->gpu_list = find_char_arg(ctx->argc, ctx->argv, "-gpus", 0);
  ctx->gpus = 0;
  ctx-> gpu = 0;
  ctx-> ngpus = 0;
  if (ctx->gpu_list) {
    printf("%s\n", ctx->gpu_list);
    int len = (int)strlen(ctx->gpu_list);
    ctx->ngpus = 1;
    int i;
    for (i = 0; i < len; ++i) {
      if (ctx->gpu_list[i] == ',') ++ctx->ngpus;
    }
    ctx->gpus = (int *)xcalloc(ctx->ngpus, sizeof(int));
    for (i = 0; i < ctx->ngpus; ++i) {
      ctx->gpus[i] = atoi(ctx->gpu_list);
      ctx->gpu_list = strchr(ctx->gpu_list, ',') + 1;
    }
  } else {
    ctx->gpu = gpu_index;
    ctx->gpus = &ctx->gpu;
    ctx->ngpus = 1;
  }

  ctx-> clear = find_arg(ctx->argc, ctx->argv, "-clear");

  ctx->datacfg = ctx->argv[3];
  ctx->cfg = ctx->argv[4];
  ctx->weights = (ctx->argc > 5) ? ctx->argv[5] : 0;

  if (ctx->weights)
    if (strlen(ctx->weights) > 0)
      if (ctx->weights[strlen(ctx->weights) - 1] == 0x0d)
        ctx->weights[strlen(ctx->weights) - 1] = 0;

  ctx->filename = (ctx->argc > 6) ? ctx->argv[6] : 0;

  return 1;
}