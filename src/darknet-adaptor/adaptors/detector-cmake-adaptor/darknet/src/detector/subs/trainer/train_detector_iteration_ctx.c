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

  unsigned int reload_now = 0;

  if (ctx->nth_iteration_data_reload > 0) {
    reload_now = ctx->current_iteration % ctx->nth_iteration_data_reload;
  }

  if (ctx->reload_data || reload_now == 0 || ctx->count == 0) {
    CLOGIM("Reloading data by request");
    free_data(ctx->train);
    ctx->load_thread = load_data(ctx->args);
    pthread_join(ctx->load_thread, 0);
    ctx->train = ctx->buffer;
  }

  pretrain_net_prepare_ctx(ctx);

  if (ctx->net.track) {
    ctx->net.sequential_subdivisions = get_current_seq_subdivisions(ctx->net);
    ctx->args.threads = ctx->net.sequential_subdivisions * ctx->ngpus;
    CLOGI(" sequential_subdivisions = %d, sequence = %d",
          ctx->net.sequential_subdivisions, get_sequence_value(ctx->net));
  }

  save_truth_image_ctx(ctx);

  init_pretrain_time_ctx(ctx);

  train_network_ctx(ctx);

  recompute_avg_loss_ctx(ctx);

  recompute_times_ctx(ctx);

  print_net_stat_ctx(ctx);

  calc_map_iteration_ctx(ctx);

  draw_train_loss_ctx(ctx);

  save_weights_ctx(ctx);

  return 1;
}
