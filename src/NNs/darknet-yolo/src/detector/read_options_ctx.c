#include "src/detector/read_options_ctx.h"

#include <stdio.h>
#include <assert.h>

void read_options_ctx(struct detector_context* ctx)
{
  assert(ctx != NULL);

  if (ctx == NULL) {
    printf("Invalid context pointer provided for train_detector_ctx\n");
    return;
  }

  ctx->options = read_data_cfg(ctx->datacfg);
  ctx->train_images = option_find_str(ctx->options, "train", "data/train.txt");
  ctx->valid_images = option_find_str(ctx->options, "valid", ctx->train_images);
  ctx->backup_directory = option_find_str(ctx->options, "backup", "/backup/");

  ctx->base = basecfg(ctx->cfg);
  printf("%s\n", ctx->base);

  ctx->save_after_iterations = option_find_int(
      ctx->options, "saveweights",
      (ctx->net.max_batches < 10000) ? 1000
                                : 10000);  // configure when to write weights.
                                           // Very useful for smaller datasets!

  ctx->save_last_weights_after = option_find_int(ctx->options, "savelast", 100);
}
