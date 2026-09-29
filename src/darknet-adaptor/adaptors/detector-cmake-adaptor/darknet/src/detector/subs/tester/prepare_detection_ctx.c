#include "src/detector/subs/tester/prepare_detection_ctx.h"

#include <assert.h>
#include <stdlib.h>

#include "image.h"
#include "option_list.h"
#include "src/clog/clog.h"
#include "src/detector/subs/tester/load_class_names_ctx.h"
#include "src/detector/subs/tester/refine_image_filename_ctx.h"
#include "src/detector/subs/trainer/traverse_detection_layers_ctx.h"

int prepare_detection_ctx(struct detector_context* ctx)
{
  assert(ctx != NULL);

  if (ctx == NULL) {
    CLOGEM("Invalid context pointer provided");
    return 0;
  }

  // the single image batch network, it's weights loaded and the batch
  // normalization fused into them, comes from the C++ network
  // infrastructure, which owns and releases it
  if (ctx->net.layers == NULL || ctx->net.n <= 0) {
    CLOGEM("No detection network provided");
    return 0;
  }

  if (ctx->datacfg != NULL) {
    CLOGD("Taking data cfg: %s", ctx->datacfg);
    ctx->options = read_data_cfg(ctx->datacfg);
  }

  if (ctx->net.letter_box) ctx->letter_box = 1;

  traverse_detection_layers_ctx(ctx);

  if (!load_class_names_ctx(ctx)) {
    CLOGEM("Failure during the class names load");
    return 0;
  }

  srand(2222222);

  if (!refine_image_filename_ctx(ctx)) {
    CLOGEM("Failure during image filename refine");
    return 0;
  }

  ctx->im = load_image(ctx->input, 0, 0, ctx->net.c);

  if (ctx->letter_box)
    ctx->sized = letterbox_image(ctx->im, ctx->net.w, ctx->net.h);
  else
    ctx->sized = resize_image(ctx->im, ctx->net.w, ctx->net.h);

  return 1;
}
