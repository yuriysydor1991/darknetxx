#include "src/detector/subs/tester/test_detector_ctx.h"

#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>

#include "image.h"
#include "network.h"
#include "option_list.h"
#include "src/clog/clog.h"
#include "src/detector/subs/tester/detect_objects_ctx.h"
#include "src/detector/subs/tester/prepare_detection_ctx.h"
#include "src/detector/subs/tester/refine_image_filename_ctx.h"
#include "src/detector/subs/tester/save_detections_json_ctx.h"
#include "src/detector/subs/tester/save_detections_labels_ctx.h"
#include "src/detector/subs/tester/show_detections_ctx.h"
#include "utils.h"

/// Releases everything the detection allocated inside the context and resets
/// the released fields, so the context never refers to a freed memory.
static void free_test_detector_ctx(struct detector_context *ctx)
{
  free_detections(ctx->dets, ctx->nboxes);
  ctx->dets = NULL;
  ctx->nboxes = 0;

  free_image(ctx->im);
  memset(&ctx->im, 0, sizeof(ctx->im));

  free_image(ctx->sized);
  memset(&ctx->sized, 0, sizeof(ctx->sized));

  free(ctx->input);
  ctx->input = NULL;

  // the class names belong to the C++ network infrastructure
  ctx->names = NULL;
  ctx->names_size = 0;

  free_list_contents_kvp(ctx->options);
  free_list(ctx->options);
  ctx->options = NULL;

  // the network belongs to the C++ network infrastructure, which releases it
  // after the detection
}

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

  // a benchmark waits for no window, the way the original Darknet did
  if (ctx->benchmark) {
    ctx->dont_show = 1;
  }

  if (!prepare_detection_ctx(ctx)) {
    CLOGEM("Failure during the detection preparation");
    free_test_detector_ctx(ctx);
    return 0;
  }

  if (detector_context_is_stopped(ctx)) {
    CLOGIM("The detection is stopped by request");
    free_test_detector_ctx(ctx);
    return 0;
  }

  float *X = ctx->sized.data;

  ctx->time = what_time_is_it_now();

  network_predict(ctx->net, X);

  // network_predict_image(&net, im); letterbox = 1;
  ctx->predict_time_ms = (what_time_is_it_now() - ctx->time) * MS_IN_S;

  CLOGI("%s: Predicted in %lf seconds", ctx->input,
        (ctx->predict_time_ms / MS_IN_S));

  if (detector_context_is_stopped(ctx)) {
    CLOGIM("The detection is stopped by request, skipping its outputs");
    free_test_detector_ctx(ctx);
    return 0;
  }

  // The detections are computed once and before any of their consumers, so
  // the JSON, the labels and the drawn image all get the same results.
  int status = detect_objects_ctx(ctx);

  if (status) {
    if (!save_detections_json_ctx(ctx)) {
      CLOGEM("Failure during the detections JSON save");
      status = 0;
    }

    if (!save_detections_labels_ctx(ctx)) {
      CLOGEM("Failure during the detections labels save");
      status = 0;
    }

    if (!show_detections_ctx(ctx)) {
      CLOGEM("Failure during the detections show");
      status = 0;
    }
  } else {
    CLOGEM("Failure during the objects detection");
  }

  free_test_detector_ctx(ctx);

  return status;
}
