#include "src/detector/train_network_ctx.h"

#include <assert.h>

#include "network.h"

void train_network_ctx(struct detector_context* ctx)
{
  assert(ctx != NULL);

#ifdef GPU
    if (ctx->ngpus == 1) {
      int wait_key = (ctx->dont_show) ? 0 : 1;
      ctx->loss = train_network_waitkey(ctx->net, ctx->train, wait_key);
    } else {
      ctx->loss = train_networks(ctx->nets, ctx->ngpus, ctx->train, 4);
    }
#else
    ctx->loss = train_network(ctx->net, ctx->train);
#endif
}