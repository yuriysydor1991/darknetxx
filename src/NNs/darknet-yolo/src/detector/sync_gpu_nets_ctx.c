#include "src/detector/sync_gpu_nets_ctx.h"

#include <assert.h>

void sync_gpu_nets_ctx(struct detector_context* ctx)
{
  assert(ctx != NULL);

#ifdef GPU
  if (ctx->ngpus != 1) sync_nets(ctx->nets, ctx->ngpus, 0);
#endif
}