#include "src/detector/subs/trainer/pretrain_net_prepare_ctx.h"

#include <stdio.h>
#include <stdlib.h>

#include "network.h"
#include "src/clog/clog.h"
#include "utils.h"

void pretrain_net_prepare_ctx(struct detector_context* ctx)
{
  assert(ctx != NULL);

  if (ctx == NULL) {
    CLOGEM("invalid context pointer provided");
    return;
  }

  if ((!(ctx->l.random && ctx->count++ % ctx->resize_network_every_nth == 0)) ||
      ctx->dont_resize_network) {
    ctx->train = ctx->buffer;
    return;
  }

  float rand_coef = 1.4;

  if (ctx->l.random != 1.0) rand_coef = ctx->l.random;

  CLOGI("Resizing, random_coef = %.2f", rand_coef);

  float random_val = rand_scale(rand_coef);  // *x or /x
  int dim_w = roundl(random_val * ctx->init_w / ctx->net.resize_step + 1) *
              ctx->net.resize_step;
  int dim_h = roundl(random_val * ctx->init_h / ctx->net.resize_step + 1) *
              ctx->net.resize_step;

  if (random_val < 1 && (dim_w > ctx->init_w || dim_h > ctx->init_h))
    dim_w = ctx->init_w, dim_h = ctx->init_h;

  int max_dim_w = roundl(rand_coef * ctx->init_w / ctx->net.resize_step + 1) *
                  ctx->net.resize_step;
  int max_dim_h = roundl(rand_coef * ctx->init_h / ctx->net.resize_step + 1) *
                  ctx->net.resize_step;

  // at the beginning (check if enough memory) and at the end (calc rolling
  // mean/variance)
  if (ctx->avg_loss < 0 ||
      ctx->current_iteration > ctx->net.max_batches - 100) {
    dim_w = max_dim_w;
    dim_h = max_dim_h;
  }

  if (dim_w < ctx->net.resize_step) dim_w = ctx->net.resize_step;
  if (dim_h < ctx->net.resize_step) dim_h = ctx->net.resize_step;

  int dim_b = (ctx->init_b * max_dim_w * max_dim_h) / (dim_w * dim_h);
  int new_dim_b = (int)(dim_b * 0.8);

  if (new_dim_b > ctx->init_b) dim_b = new_dim_b;

  ctx->args.w = dim_w;
  ctx->args.h = dim_h;

  if (ctx->net.dynamic_minibatch) {
    for (int k = 0; k < ctx->ngpus; ++k) {
      (*ctx->nets[k].seen) =
          ctx->init_b * ctx->net.subdivisions *
          ctx->current_iteration;  // remove this line, when you will save to
                                   // weights-file both: seen & cur_iteration
      ctx->nets[k].batch = dim_b;
      for (int j = 0; j < ctx->nets[k].n; ++j)
        ctx->nets[k].layers[j].batch = dim_b;
    }

    ctx->net.batch = dim_b;
    ctx->imgs = ctx->net.batch * ctx->net.subdivisions * ctx->ngpus;
    ctx->args.n = ctx->imgs;

    CLOGI("%d x %d  (batch = %d)", dim_w, dim_h, ctx->net.batch);
  } else
    CLOGI("%d x %d", dim_w, dim_h);

  // pthread_join(ctx->load_thread, 0);

  ctx->train = ctx->buffer;

  free_data(ctx->train);

  ctx->load_thread = load_data(ctx->args);

  for (int k = 0; k < ctx->ngpus; ++k) {
    resize_network(ctx->nets + k, dim_w, dim_h);
  }

  ctx->net = ctx->nets[0];

  pthread_join(ctx->load_thread, 0);
  ctx->train = ctx->buffer;
}
