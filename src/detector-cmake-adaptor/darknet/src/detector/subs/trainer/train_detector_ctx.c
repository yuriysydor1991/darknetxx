#include "src/detector/subs/trainer/train_detector_ctx.h"

#include <assert.h>
#include <stdio.h>

#include "src/clog/clog.h"
#include "src/detector/subs/trainer/calc_map_free_ctx.h"
#include "src/detector/subs/trainer/calc_map_init_ctx.h"
#include "src/detector/subs/trainer/calc_map_iteration_ctx.h"
#include "src/detector/subs/trainer/clear_context.h"
#include "src/detector/subs/trainer/draw_train_chart_ctx.h"
#include "src/detector/subs/trainer/draw_train_loss_ctx.h"
#include "src/detector/subs/trainer/init_pretrain_time_ctx.h"
#include "src/detector/subs/trainer/prepare_load_args_ctx.h"
#include "src/detector/subs/trainer/prepare_train_ctx.h"
#include "src/detector/subs/trainer/print_net_init_stats_ctx.h"
#include "src/detector/subs/trainer/print_net_stat_ctx.h"
#include "src/detector/subs/trainer/recompute_avg_loss_ctx.h"
#include "src/detector/subs/trainer/recompute_times_ctx.h"
#include "src/detector/subs/trainer/save_weights_best_ctx.h"
#include "src/detector/subs/trainer/save_weights_ctx.h"
#include "src/detector/subs/trainer/save_weights_final_ctx.h"
#include "src/detector/subs/trainer/sync_gpu_nets_ctx.h"
#include "src/detector/subs/trainer/train_detector_iteration_ctx.h"
#include "src/detector/subs/trainer/train_network_ctx.h"
#include "src/detector/subs/trainer/traverse_detection_layers_ctx.h"
#include "src/parser.h"
#include "src/utils.h"

int train_detector_ctx(struct detector_context *ctx)
{
  assert(ctx != NULL);

  static const double default_lr = 0.0013;

  if (ctx == NULL) {
    CLOGEM("Invalid context pointer provided for train_detector_ctx");
    return 0;
  }

  srand(time(0));

  if (!prepare_train_ctx(ctx)) {
    clear_context(ctx);
    CLOGEM("Fail to prepare the training");
    return 0;
  }

  // the learning rate comes from the just parsed cfg of every network copy
  for (int k = 0; k < ctx->ngpus; ++k) {
    if (ctx->nets[k].learning_rate == 0.0f) {
      CLOGW("Invalid learning rate provided to the net, setting up default %f",
            default_lr);
      ctx->nets[k].learning_rate = default_lr * ctx->ngpus;
    }
  }

  ctx->net = ctx->nets[0];

  // takes the validation images and names from the just parsed network
  if (!calc_map_init_ctx(ctx)) {
    clear_context(ctx);
    CLOGEM("Fail to prepare the mAP calculation");
    return 0;
  }

  srand(time(0));

  print_net_init_stats_ctx(ctx);

  traverse_detection_layers_ctx(ctx);

  prepare_load_args_ctx(ctx);

  draw_train_chart_ctx(ctx);

  int loss_limit_reached = 0;

  // a stop request before any training iteration has no progress to keep
  const int start_iteration = get_current_iteration(ctx->net);

  while ((ctx->current_iteration = get_current_iteration(ctx->net)) <
             ctx->net.max_batches &&
         !detector_context_is_stopped(ctx)) {
    if (!train_detector_iteration_ctx(ctx)) {
      CLOGEM("Failure during the train iteration");
      save_train_chart_ctx(ctx);
      clear_context(ctx);
      return 0;
    }

    if (!isfinite(ctx->avg_loss)) {
      CLOGEM("Avg loss is not a valid number in");
      save_train_chart_ctx(ctx);
      clear_context(ctx);
      return 0;
    }

    if (ctx->stop_less_avg_loss > 0.0 &&
        ctx->stop_less_avg_loss >= ctx->avg_loss) {
      CLOGI("The avg loss limit reached %g >= %g", ctx->stop_less_avg_loss,
            ctx->avg_loss);
      loss_limit_reached = 1;
      break;
    }
  }

  sync_gpu_nets_ctx(ctx);

  // The reached loss limit is the requested result of the training as much as
  // the last iteration is, while a stop request only cuts the training short,
  // so its progress is kept as the last weights instead of the final ones.
  if (loss_limit_reached ||
      get_current_iteration(ctx->net) >= ctx->net.max_batches) {
    save_weights_final_ctx(ctx);
  } else if (get_current_iteration(ctx->net) > start_iteration) {
    CLOGIM("The training is stopped by request, saving the last weights");
    save_weights_last_ctx(ctx);
  } else {
    // the last weights of a previous run stay intact
    CLOGIM(
        "The training is stopped by request before any iteration, no weights "
        "to save");
  }

  // the points since the last periodic save of the chart are kept too
  save_train_chart_ctx(ctx);

  CLOGIM(
      "If you want to train from the beginning, then use flag in the end of "
      "training command: --clear");

  clear_context(ctx);

  return 1;
}
