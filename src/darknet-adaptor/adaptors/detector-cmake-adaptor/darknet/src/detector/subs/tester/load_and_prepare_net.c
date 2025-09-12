#include "src/detector/subs/tester/load_and_prepare_net.h"

#include <assert.h>

#include "image.h"
#include "option_list.h"
#include "parser.h"
#include "src/clog/clog.h"
#include "src/detector/subs/tester/refine_image_filename_ctx.h"
#include "src/detector/subs/trainer/traverse_detection_layers_ctx.h"

int load_and_prepare_net(struct detector_context* ctx)
{
  assert(ctx != NULL);

  if (ctx == NULL) {
    CLOGEM("Invalid context pointer provided");
    return 0;
  }

  ctx->net = parse_network_cfg_custom(ctx->cfg, 1, 1);  // set batch=1

  if (ctx->datacfg != NULL) {
    CLOGD("Taking data cfg: %s", ctx->datacfg);
    ctx->options = read_data_cfg(ctx->datacfg);
  }

  load_weights(&ctx->net, ctx->weights);

  if (ctx->net.letter_box) ctx->letter_box = 1;

  ctx->net.benchmark_layers = ctx->benchmark_layers;

  fuse_conv_batchnorm(ctx->net);
  calculate_binary_weights(ctx->net);

  traverse_detection_layers_ctx(ctx);

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
