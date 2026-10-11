#include "src/detector/subs/trainer/calc_map_free_ctx.h"

#include <assert.h>
#include <string.h>

void calc_map_free_ctx(struct detector_context* ctx)
{
  assert(ctx != NULL);

  if (!ctx->calc_map) {
    return;
  }

  // The mAP calculation network belongs to the C++ network infrastructure,
  // which releases it after the training, so only the view is dropped here.
  memset(&ctx->net_map, 0, sizeof(ctx->net_map));
}
