#include "src/detector/subs/trainer/recompute_avg_loss_ctx.h"

#include <assert.h>

void recompute_avg_loss_ctx(struct detector_context* ctx)
{
  assert(ctx != NULL);

  if (ctx->avg_loss < 0 || ctx->avg_loss != ctx->avg_loss) {
    ctx->avg_loss = ctx->loss;  // if(-inf or nan)
  }

  ctx->avg_loss = ctx->avg_loss * .9 + ctx->loss * .1;
}