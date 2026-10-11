#include "src/detector/subs/trainer/save_weights_ctx.h"

#include <assert.h>
#include <limits.h>
#include <stdio.h>

#include "network.h"
#include "src/clog/clog.h"
#include "src/detector/subs/trainer/save_train_weights_ctx.h"
#include "src/detector/subs/trainer/sync_gpu_nets_ctx.h"

void save_weights_ctx(struct detector_context* ctx)
{
  assert(ctx != NULL);

  if (ctx == NULL) {
    CLOGEM("invalid train context pointer provided");
    return;
  }

  const int iteration = ctx->current_iteration;

  if ((iteration >= (ctx->iter_save + ctx->net.saveweights) ||
       iteration % ctx->net.saveweights == 0)) {
    ctx->iter_save = iteration;
    sync_gpu_nets_ctx(ctx);
    char stem[PATH_MAX];
    snprintf(stem, sizeof(stem), "%s/%s_%d_%f", ctx->net.backup_directory,
             ctx->base, iteration, ctx->avg_loss);
    save_train_weights_ctx(ctx, stem, 0);
  }

  if ((ctx->net.saveweights > ctx->net.savelast) &&
      (iteration >= (ctx->iter_save_last + ctx->net.savelast) ||
       (iteration % ctx->net.savelast == 0 && iteration > 1))) {
    ctx->iter_save_last = iteration;
    save_weights_last_ctx(ctx);
  }
}

void save_weights_last_ctx(struct detector_context* ctx)
{
  assert(ctx != NULL);

  sync_gpu_nets_ctx(ctx);
  char stem[PATH_MAX];
  snprintf(stem, sizeof(stem), "%s/%s_last", ctx->net.backup_directory,
           ctx->base);
  save_train_weights_ctx(ctx, stem, 0);

  if (ctx->net.ema_alpha && is_ema_initialized(ctx->net)) {
    snprintf(stem, sizeof(stem), "%s/%s_ema", ctx->net.backup_directory,
             ctx->base);
    save_train_weights_ctx(ctx, stem, 1);
    CLOGI(" EMA weights are saved to the file: %s", stem);
  }
}
