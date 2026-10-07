#include "src/detector/subs/trainer/save_weights_final_ctx.h"

#include <assert.h>
#include <limits.h>
#include <stdio.h>

#include "src/detector/subs/trainer/save_train_weights_ctx.h"

void save_weights_final_ctx(struct detector_context* ctx)
{
  assert(ctx != NULL);

  char stem[PATH_MAX];

  snprintf(stem, sizeof(stem), "%s/%s_final", ctx->net.backup_directory,
           ctx->base);

  save_train_weights_ctx(ctx, stem, 0);
}
