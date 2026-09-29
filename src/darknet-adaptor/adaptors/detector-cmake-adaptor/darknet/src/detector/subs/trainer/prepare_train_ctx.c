#include "src/detector/subs/trainer/prepare_train_ctx.h"

#include <assert.h>
#include <stdlib.h>

#include "data.h"
#include "memory.h"
#include "network.h"
#include "src/clog/clog.h"
#include "utils.h"

int prepare_train_ctx(struct detector_context* ctx)
{
  static const unsigned int DEFAULT_NTH_RESIZE = 10;

  assert(ctx != NULL);

  if (ctx == NULL) {
    return 0;
  }

  // the networks, their weights included, are loaded by the C++ network
  // infrastructure, which owns and releases them
  if (ctx->nets == NULL || ctx->ngpus <= 0) {
    CLOGEM("No training networks provided");
    return 0;
  }

  ctx->base = basecfg(ctx->cfg);
  CLOGI("The BASE: %s", ctx->base);

  ctx->avg_loss = -1;
  ctx->avg_contrastive_acc = 0;

  ctx->net = ctx->nets[0];

  // a resumed training continues from the iteration of the loaded weights
  ctx->current_iteration = get_current_iteration(ctx->net);
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
