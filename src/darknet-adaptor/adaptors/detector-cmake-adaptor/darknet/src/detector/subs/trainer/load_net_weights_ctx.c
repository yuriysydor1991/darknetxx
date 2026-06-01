#include "src/detector/subs/trainer/load_net_weights_ctx.h"

#include <assert.h>
#include <stdlib.h>

#include "data.h"
#include "memory.h"
#include "parser.h"
#include "src/clog/clog.h"
#include "utils.h"

int load_net_weights_ctx(struct detector_context* ctx)
{
  static const unsigned int DEFAULT_NTH_RESIZE = 10;

  assert(ctx != NULL);

  if (ctx == NULL) {
    return 0;
  }

  ctx->base = basecfg(ctx->cfg);
  CLOGI("The BASE: %s", ctx->base);

  ctx->avg_loss = -1;
  ctx->avg_contrastive_acc = 0;

  if (ctx->ngpus <= 0) {
    CLOGWM("The ngpus value should contain at least value 1, setting it up");
    ctx->ngpus = 1;
  }

  ctx->nets = (network*)xcalloc(ctx->ngpus, sizeof(network));

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

  ctx->iter_save = ctx->current_iteration;
  ctx->iter_save_last = ctx->current_iteration;
  ctx->iter_map = ctx->current_iteration;
  ctx->mean_average_precision = -1;
  ctx->best_map = ctx->mean_average_precision;

  ctx->actual_batch_size = ctx->net.batch * ctx->net.subdivisions;

  ctx->avg_time = -1;
  ctx->alpha_time = 0.01;

  ctx->imgs = ctx->net.batch * ctx->net.subdivisions * ctx->ngpus;

  if (ctx->net.train_images == NULL) {
    CLOGEM("No train_images file provided");
    return 0;
  }

  ctx->plist = get_paths(ctx->net.train_images);
  ctx->train_images_num = ctx->plist->size;
  ctx->paths = (char**)list_to_array(ctx->plist);

  CLOGI("Loaded train images count: %d", ctx->train_images_num);

  ctx->init_w = ctx->net.w;
  ctx->init_h = ctx->net.h;
  ctx->init_b = ctx->net.batch;

  ctx->count = 0;

  ctx->resize_network_every_nth = ctx->resize_network_every_nth > 0
                                      ? ctx->resize_network_every_nth
                                      : DEFAULT_NTH_RESIZE;

  CLOGI("Using every %dth train iteration net resize",
        ctx->resize_network_every_nth);

  return 1;
}
