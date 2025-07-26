#include "src/detector/print_net_init_stats_ctx.h"

#include <stdio.h>
#include <assert.h>

#include "utils.h"

void print_net_init_stats_ctx(struct detector_context* ctx)
{
  assert(ctx != NULL);
  
  if (ctx == NULL) {
    return;
  }

  if (ctx->actual_batch_size == 1) {
    error(
        "Error: You set incorrect value batch=1 for Training! You should set "
        "batch=64 subdivision=64",
        DARKNET_LOC);
  } else if (ctx->actual_batch_size < 8) {
    printf(
        "\n Warning: You set batch=%d lower than 64! It is recommended to set "
        "batch=64 subdivision=64 \n",
        ctx->actual_batch_size);
  }

  printf(
      "Weights are saved after: %d iterations. Last weights (*_last.weight) "
      "are stored every %d iterations. \n",
      ctx->save_after_iterations, ctx->save_last_weights_after);

  printf("Learning Rate: %g, Momentum: %g, Decay: %g\n", ctx->net.learning_rate,
          ctx->net.momentum, ctx->net.decay);
}