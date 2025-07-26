#include "src/detector/calc_map_free_ctx.h"
#include <assert.h>

void calc_map_free_ctx(struct detector_context* ctx)
{
  assert(ctx != NULL);

  if (!ctx->calc_map) {
    return;
  }

  ctx->net_map.n = 0;
  free_network(ctx->net_map);
}
