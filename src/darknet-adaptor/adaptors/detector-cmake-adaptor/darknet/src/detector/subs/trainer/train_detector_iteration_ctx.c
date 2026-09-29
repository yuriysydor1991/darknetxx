#include "src/detector/subs/trainer/train_detector_iteration_ctx.h"

#include <assert.h>

#include "data.h"
#include "network.h"
#include "src/clog/clog.h"
#include "src/detector/subs/trainer/calc_map_iteration_ctx.h"
#include "src/detector/subs/trainer/draw_train_loss_ctx.h"
#include "src/detector/subs/trainer/init_pretrain_time_ctx.h"
#include "src/detector/subs/trainer/pretrain_net_prepare_ctx.h"
#include "src/detector/subs/trainer/print_net_stat_ctx.h"
#include "src/detector/subs/trainer/recompute_avg_loss_ctx.h"
#include "src/detector/subs/trainer/recompute_times_ctx.h"
#include "src/detector/subs/trainer/save_truth_image.h"
#include "src/detector/subs/trainer/save_weights_ctx.h"
#include "src/detector/subs/trainer/train_data_ctx.h"
#include "src/detector/subs/trainer/train_network_ctx.h"
#include "src/parser.h"
#include "src/utils.h"

int train_detector_iteration_ctx(struct detector_context* ctx)
{
  assert(ctx != NULL);

  if (ctx == NULL) {
    CLOGEM("Invalid context pointer provided");
    return 0;
  }

  ctx->time = what_time_is_it_now();

  const int iteration = ctx->current_iteration;

  // A resized network needs a batch of its new input size, which a batch
  // loaded in advance for the old size isn't.
  if (pretrain_net_prepare_ctx(ctx)) {
    drop_train_data_load_ctx(ctx);
    ctx->data_reload_forced = 1;
  }

  if (ctx->data_reload_forced || ctx->train.X.rows == 0 ||
      train_data_reload_due_ctx(ctx, iteration)) {
    take_train_data_ctx(ctx);
    ctx->data_reload_forced = 0;
  }

  if (ctx->net.track) {
    ctx->net.sequential_subdivisions = get_current_seq_subdivisions(ctx->net);
    ctx->args.threads = ctx->net.sequential_subdivisions * ctx->ngpus;
    CLOGI(" sequential_subdivisions = %d, sequence = %d",
          ctx->net.sequential_subdivisions, get_sequence_value(ctx->net));
  }

  // The batch of the next iteration loads while the current one trains,
  // unless the next iteration resizes the network (the pretrain_net_prepare_ctx
  // counter check) and so needs a batch of another size anyway.
  const int next_resizes = ctx->l.random && !ctx->dont_resize_network &&
                           ctx->count % ctx->resize_network_every_nth == 0;

  if (!next_resizes && train_data_reload_due_ctx(ctx, iteration + 1)) {
    start_train_data_load_ctx(ctx);
  }

  save_truth_image_ctx(ctx);

  init_pretrain_time_ctx(ctx);

  train_network_ctx(ctx);

  // The steps below report, evaluate and save the network the iteration just
  // trained, so they take the iteration number after the training the way the
  // original Darknet did (the ones above took the number before it).
  ctx->current_iteration = get_current_iteration(ctx->net);

  recompute_avg_loss_ctx(ctx);

  recompute_times_ctx(ctx);

  print_net_stat_ctx(ctx);

  calc_map_iteration_ctx(ctx);

  draw_train_loss_ctx(ctx);

  save_weights_ctx(ctx);

  return 1;
}
