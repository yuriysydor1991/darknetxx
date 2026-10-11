#include <assert.h>

#include "parser.h"
#include "src/clog/clog.h"
#include "src/detector/subs/trainer/calc_map_init_ctx.h"
#include "src/detector/subs/trainer/save_weights_best_ctx.h"
#include "src/detector/subs/trainer/train_data_ctx.h"
#include "src/utils.h"

void calc_map_iteration_ctx(struct detector_context *ctx)
{
  assert(ctx != NULL);

  if (ctx == NULL) {
    CLOGEM("Invalid context pointer provided");
    return;
  }

  const int iteration = ctx->current_iteration;

  ctx->map_calculated = 0;

  if (!(ctx->calc_map && (iteration >= ctx->next_map_calc ||
                          iteration == ctx->net.max_batches))) {
    return;
  }

  if (ctx->l.random) {
    CLOGI("Resizing to initial size: %d x %d", ctx->init_w, ctx->init_h);

    ctx->args.w = ctx->init_w;
    ctx->args.h = ctx->init_h;

    if (ctx->net.dynamic_minibatch) {
      for (int k = 0; k < ctx->ngpus; ++k) {
        ctx->nets[k].batch = ctx->init_b;
        for (int j = 0; j < ctx->nets[k].n; ++j)
          ctx->nets[k].layers[j].batch = ctx->init_b;
      }

      ctx->net.batch = ctx->init_b;
      ctx->imgs = ctx->init_b * ctx->net.subdivisions * ctx->ngpus;
      ctx->args.n = ctx->imgs;

      CLOGI(" %d x %d  (batch = %d)", ctx->init_w, ctx->init_h, ctx->init_b);
    }

    // The batch loaded in advance for the current input size doesn't fit the
    // network of the initial one, so the next iteration loads a fresh batch.
    drop_train_data_load_ctx(ctx);
    ctx->data_reload_forced = 1;

    for (int k = 0; k < ctx->ngpus; ++k) {
      resize_network(ctx->nets + k, ctx->init_w, ctx->init_h);
    }

    ctx->net = ctx->nets[0];
  }

  copy_weights_net(ctx->net, &ctx->net_map);

  // combine Training and Validation networks
  // network net_combined = combine_train_valid_networks(net, net_map);

  ctx->iter_map = iteration;
  ctx->mean_average_precision = validate_detector_map(
      ctx->datacfg, ctx->cfg, ctx->weights, ctx->thresh, ctx->iou_thresh,
      ctx->map_points, ctx->net.letter_box, &ctx->net_map, ctx->names,
      ctx->names_size);

  CLOGI(" mean_average_precision (mAP@%0.2f) = %f", ctx->iou_thresh,
        ctx->mean_average_precision);

  // the best weights saved keep the mAP point of their own
  ctx->map_calculated = 1;

  save_weights_best_ctx(ctx);
}
