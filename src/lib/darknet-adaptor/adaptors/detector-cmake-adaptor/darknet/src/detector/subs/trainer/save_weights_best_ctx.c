#include "src/detector/subs/trainer/save_weights_best_ctx.h"

#include <assert.h>
#include <limits.h>
#include <stdio.h>

#include "src/clog/clog.h"
#include "src/detector/subs/trainer/save_train_weights_ctx.h"

void save_weights_best_ctx(struct detector_context* ctx)
{
  assert(ctx != NULL);

  if (ctx->mean_average_precision < ctx->best_map) {
    return;
  }

  ctx->best_map = ctx->mean_average_precision;
  CLOGIM("New best mAP!");

  char stem[PATH_MAX];

  snprintf(stem, sizeof(stem), "%s/%s_best", ctx->net.backup_directory,
           ctx->base);

  save_train_weights_ctx(ctx, stem, 0);
}
