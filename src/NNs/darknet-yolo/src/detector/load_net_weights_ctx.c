#include "src/detector/load_net_weights_ctx.h"

#include <assert.h>
#include <stdlib.h>

#include "parser.h"

void load_net_weights_ctx(struct detector_context* ctx)
{
  assert(ctx != NULL);
  
  if (ctx == NULL) {
    return;
  }

  ctx->avg_loss = -1;
  ctx->avg_contrastive_acc = 0;

  ctx->nets = (network *)xcalloc(ctx->ngpus, sizeof(network));

  srand(time(0));

  int seed = rand();

  for (int k = 0; k < ctx->ngpus; ++k) {
    srand(seed);
#ifdef GPU
    cuda_set_device(gpus[k]);
#endif
    ctx->nets[k] = parse_network_cfg(ctx->cfg);
    ctx->nets[k].benchmark_layers = ctx->benchmark_layers;
    if (ctx->weights) {
      load_weights(&ctx->nets[k], ctx->weights);
    }
    if (ctx->clear) {
      *ctx->nets[k].seen = 0;
      *ctx->nets[k].cur_iteration = 0;
    }
    ctx->nets[k].learning_rate *= ctx->ngpus;
  }

  ctx->net = ctx->nets[0];

  ctx->iter_save = get_current_iteration(ctx->net);
  ctx->iter_save_last = get_current_iteration(ctx->net);
  ctx->iter_map = get_current_iteration(ctx->net);
  ctx->mean_average_precision = -1;
  ctx->best_map = ctx->mean_average_precision;

  ctx->actual_batch_size = ctx->net.batch * ctx->net.subdivisions;

  ctx->avg_time = -1;
  ctx->alpha_time = 0.01;

  ctx->imgs = ctx->net.batch * ctx->net.subdivisions * ctx->ngpus;
}
