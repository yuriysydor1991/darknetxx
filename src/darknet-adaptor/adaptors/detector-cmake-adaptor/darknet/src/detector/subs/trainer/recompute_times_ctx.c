#include "src/detector/subs/trainer/recompute_times_ctx.h"

#include <assert.h>

#include "utils.h"

void recompute_times_ctx(struct detector_context* ctx)
{
  assert(ctx != NULL);

  const int iteration = ctx->current_iteration;

  ctx->time_remaining = ((ctx->net.max_batches - iteration) / ctx->ngpus) *
                        (what_time_is_it_now() - ctx->time + ctx->load_time) /
                        60 / 60;
  // set initial value, even if resume training from 10000 iteration
  if (ctx->avg_time < 0) ctx->avg_time = ctx->time_remaining;
  else
    ctx->avg_time = ctx->alpha_time * ctx->time_remaining +
                    (1 - ctx->alpha_time) * ctx->avg_time;
}
