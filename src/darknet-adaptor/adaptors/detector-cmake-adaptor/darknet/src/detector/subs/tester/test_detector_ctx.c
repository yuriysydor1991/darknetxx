#include "src/detector/subs/tester/test_detector_ctx.h"

#include <assert.h>
#include <limits.h>
#include <stdio.h>

#include "image.h"
#include "option_list.h"
#include "parser.h"
#include "src/clog/clog.h"
#include "src/detector/subs/tester/load_and_prepare_net.h"
#include "src/detector/subs/tester/refine_image_filename_ctx.h"
#include "src/detector/subs/tester/save_detections_json_ctx.h"
#include "src/detector/subs/tester/save_detections_labels_ctx.h"
#include "src/detector/subs/tester/show_detections_ctx.h"
#include "utils.h"

int test_detector_ctx(struct detector_context *ctx)
{
  static const double MS_IN_S = 1000.f;

  assert(ctx != NULL);

  if (!ctx->filename) {
    CLOGEM("No image filename to analysis provided");
    return 0;
  }

  if (ctx->cfg == NULL) {
    CLOGEM("No network cfg file path provided");
    return 0;
  }

  if (ctx->weights == NULL) {
    CLOGEM("No network weights file path provided");
    return 0;
  }

  if (!load_and_prepare_net(ctx)) {
    CLOGEM("Failure during network load and prepare");
    return 0;
  }

  float *X = ctx->sized.data;

  ctx->time = what_time_is_it_now();

  network_predict(ctx->net, X);

  // network_predict_image(&net, im); letterbox = 1;
  ctx->predict_time_ms = (what_time_is_it_now() - ctx->time) * MS_IN_S;

  CLOGI("%s: Predicted in %lf seconds", ctx->input,
        (ctx->predict_time_ms / MS_IN_S));

  save_detections_json_ctx(ctx);
  save_detections_labels_ctx(ctx);
  show_detections_ctx(ctx);

  // free memory
  free(ctx->input);
  free_ptrs((void **)ctx->names, ctx->names_size);
  free_list_contents_kvp(ctx->options);
  free_list(ctx->options);
  free_network(ctx->net);

  return 1;
}
