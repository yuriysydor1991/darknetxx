#include "src/detector/subs/trainer/init_pretrain_time_ctx.h"

#include <assert.h>
#include <stdio.h>

#include "utils.h"

void init_pretrain_time_ctx(struct detector_context* ctx)
{
  assert(ctx != NULL);

  ctx->load_time = (what_time_is_it_now() - ctx->time);

  ctx->time = what_time_is_it_now();
  ctx->loss = 0;
}