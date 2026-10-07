#include "src/detector/subs/trainer/print_net_init_stats_ctx.h"

#include <assert.h>
#include <stdio.h>

#include "memory.h"
#include "src/clog/clog.h"
#include "utils.h"

void print_net_init_stats_ctx(struct detector_context* ctx)
{
  assert(ctx != NULL);

  if (ctx == NULL) {
    CLOGEM("Invalid context pointer provided");
    return;
  }

  if (ctx->actual_batch_size == 1) {
    error(
        "Error: You set incorrect value batch=1 for Training! You should set "
        "batch=64 subdivision=64",
        DARKNET_LOC);
  } else if (ctx->actual_batch_size < 8) {
    CLOGW(
        "\n Warning: You set batch=%d lower than 64! It is recommended to set "
        "batch=64 subdivision=64",
        ctx->actual_batch_size);
  }

  CLOGI(
      "Weights are saved after: %d iterations. Last weights (*_last.weight) "
      "are stored every %d iterations.",
      ctx->net.saveweights, ctx->net.savelast);

  CLOGI("Learning Rate: %g, Momentum: %g, Decay: %g", ctx->net.learning_rate,
        ctx->net.momentum, ctx->net.decay);
}
