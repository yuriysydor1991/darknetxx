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
}
