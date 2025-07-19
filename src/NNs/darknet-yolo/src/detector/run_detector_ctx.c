#include "src/detector/run_detector_ctx.h"

#include <stdio.h>
#include <assert.h>

#include "src/detector/detector_context.h"
#include "src/list.h"

int run_detector_ctx(struct detector_context *ctx)
{
  assert(ctx != NULL);

  if (ctx == NULL) {
    printf("No valid context provided");
    return 0;
  }

  if (0 == strcmp(ctx->argv[2], "test"))
    test_detector(ctx->datacfg, ctx->cfg, ctx->weights, ctx->filename,
                  ctx->thresh, ctx->hier_thresh, ctx->dont_show,
                  ctx->ext_output, ctx->save_labels, ctx->outfile,
                  ctx->letter_box, ctx->benchmark_layers);
  else if (0 == strcmp(ctx->argv[2], "train"))
    train_detector(ctx->datacfg, ctx->cfg, ctx->weights, ctx->gpus, ctx->ngpus,
                   ctx->clear, ctx->dont_show, ctx->calc_map, ctx->thresh,
                   ctx->iou_thresh, ctx->mjpeg_port, ctx->show_imgs,
                   ctx->benchmark_layers, ctx->chart_path, ctx->mAP_epochs);
  else if (0 == strcmp(ctx->argv[2], "valid"))
    validate_detector(ctx->datacfg, ctx->cfg, ctx->weights, ctx->outfile);
  else if (0 == strcmp(ctx->argv[2], "recall"))
    validate_detector_recall(ctx->datacfg, ctx->cfg, ctx->weights);
  else if (0 == strcmp(ctx->argv[2], "map"))
    validate_detector_map(ctx->datacfg, ctx->cfg, ctx->weights, ctx->thresh,
                          ctx->iou_thresh, ctx->map_points, ctx->letter_box,
                          NULL);
  else if (0 == strcmp(ctx->argv[2], "calc_anchors"))
    calc_anchors(ctx->datacfg, ctx->num_of_clusters, ctx->width, ctx->height,
                 ctx->show);
  else if (0 == strcmp(ctx->argv[2], "draw")) {
    int it_num = 100;
    draw_object(ctx->datacfg, ctx->cfg, ctx->weights, ctx->filename,
                ctx->thresh, ctx->dont_show, it_num, ctx->letter_box,
                ctx->benchmark_layers);
  } else if (0 == strcmp(ctx->argv[2], "demo")) {
    list *options = read_data_cfg(ctx->datacfg);
    int classes = option_find_int(options, "classes", 20);
    char *name_list = option_find_str(options, "names", "data/names.list");
    char **names = get_labels(name_list);
    if (ctx->filename)
      if (strlen(ctx->filename) > 0)
        if (ctx->filename[strlen(ctx->filename) - 1] == 0x0d)
          ctx->filename[strlen(ctx->filename) - 1] = 0;
    demo(ctx->cfg, ctx->weights, ctx->thresh, ctx->hier_thresh, ctx->cam_index,
         ctx->filename, names, classes, ctx->avgframes, ctx->frame_skip,
         ctx->prefix, ctx->out_filename, ctx->mjpeg_port, ctx->dontdraw_bbox,
         ctx->json_port, ctx->dont_show, ctx->ext_output, ctx->letter_box,
         ctx->time_limit_sec, ctx->http_post_host, ctx->benchmark,
         ctx->benchmark_layers, ctx->json_file_output);

    free_list_contents_kvp(options);
    free_list(options);
  } else {
    printf(" There isn't such command: %s", ctx->argv[2]);
    return 0;
  }

  if (ctx->gpus && ctx->gpu_list && ctx->ngpus > 1) free(ctx->gpus);

  return 1;
}