#include "src/detector/subs/trainer/save_weights_best_ctx.h"

#include <assert.h>
#include <limits.h>
#include <stdlib.h>

#include "parser.h"
#include "src/clog/clog.h"
#include "src/detector/subs/trainer/sync_gpu_nets_ctx.h"

void save_weights_best_ctx(struct detector_context* ctx)
{
  assert(ctx != NULL);

  if (ctx->mean_average_precision < ctx->best_map) {
    return;
  }

  ctx->best_map = ctx->mean_average_precision;
  CLOGIM("New best mAP!");

  char buff[PATH_MAX];
  sprintf(buff, "%s/%s_best.weights", ctx->net.backup_directory, ctx->base);

  save_weights(ctx->net, buff);
}
