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
#include "src/detector/subs/trainer/load_net_weights_ctx.h"
#include "src/detector/subs/trainer/prepare_load_args_ctx.h"
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

  if (ctx->net.learning_rate == 0.0f) {
    CLOGW("Invalid learning rate provided to the net, setting up default %f",
          default_lr);
    ctx->net.learning_rate = default_lr;
  }

  calc_map_init_ctx(ctx);

  srand(time(0));

  if (!load_net_weights_ctx(ctx)) {
    clear_context(ctx);
    CLOGEM("Fail to load network weights");
    return 0;
  }

  srand(time(0));

  print_net_init_stats_ctx(ctx);

  traverse_detection_layers_ctx(ctx);

  prepare_load_args_ctx(ctx);

  draw_train_chart_ctx(ctx);

  while ((ctx->current_iteration = get_current_iteration(ctx->net)) <
             ctx->net.max_batches &&
         !ctx->stop) {
    if (!train_detector_iteration_ctx(ctx)) {
      CLOGEM("Failure during the train iteration");
      clear_context(ctx);
      return 0;
    }

    if (!isfinite(ctx->avg_loss)) {
      CLOGEM("Avg loss is not a valid number in");
      clear_context(ctx);
      return 0;
    }

    if (ctx->stop_less_avg_loss > 0.0 &&
        ctx->stop_less_avg_loss >= ctx->avg_loss) {
      CLOGI("The avg loss limit reached %g >= %g", ctx->stop_less_avg_loss,
            ctx->avg_loss);
      break;
    }
  }

  sync_gpu_nets_ctx(ctx);

  save_weights_final_ctx(ctx);

  CLOGIM(
      "If you want to train from the beginning, then use flag in the end of "
      "training command: -clear");

  clear_context(ctx);

  return 1;
}
