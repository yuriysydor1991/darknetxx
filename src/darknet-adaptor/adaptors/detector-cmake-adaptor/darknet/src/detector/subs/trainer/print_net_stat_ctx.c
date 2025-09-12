#include "src/detector/subs/trainer/print_net_stat_ctx.h"

#include <assert.h>
#include <stdio.h>

#include "network.h"
#include "src/clog/clog.h"
#include "utils.h"

void print_net_stat_ctx(struct detector_context* ctx)
{
  assert(ctx != NULL);

  if (ctx == NULL) {
    CLOGEM("Invalid function pointer provided");
    return;
  }

  const int iteration = ctx->current_iteration;
  // i = get_current_batch(net);

  int calc_map_for_each =
      ctx->mAP_epochs * ctx->train_images_num /
      (ctx->net.batch *
       ctx->net.subdivisions);  // calculate mAP every mAP_epochs
  calc_map_for_each = fmax(calc_map_for_each, 100);
  ctx->next_map_calc = ctx->iter_map + calc_map_for_each;
  ctx->next_map_calc = fmax(ctx->next_map_calc, ctx->net.burn_in);
  // next_map_calc = fmax(next_map_calc, 400);

  if (ctx->calc_map) {
    CLOGI("(next mAP calculation at %d iterations) ", ctx->next_map_calc);
    if (ctx->mean_average_precision > 0)
      CLOGI("Last accuracy mAP@%0.2f = %2.2f %%, best = %2.2f %% ",
            ctx->iou_thresh, ctx->mean_average_precision * 100,
            ctx->best_map * 100);
  }

  if (ctx->mean_average_precision > 0.0) {
    CLOGI("%d/%d: loss=%0.1f map=%0.2f best=%0.2f hours left=%0.1f", iteration,
          ctx->net.max_batches, ctx->loss, ctx->mean_average_precision,
          ctx->best_map, ctx->avg_time);
  } else {
    CLOGI("%d/%d: loss=%0.1f hours left=%0.1f", iteration, ctx->net.max_batches,
          ctx->loss, ctx->avg_time);
  }

  if (ctx->net.cudnn_half) {
    if (iteration < ctx->net.burn_in * 3)
      CLOGE(
          " Tensor Cores are disabled until the first %d iterations "
          "are reached.",
          3 * ctx->net.burn_in);
    else
      CLOGEM(" Tensor Cores are used.\n");
  }

  const size_t netresizebuffs = 128;
  char netresizebuff[netresizebuffs + 1];

  memset(netresizebuff, 0, netresizebuffs + 1);

  if (ctx->dont_resize_network) {
    if (ctx->nth_iteration_data_reload > 0) {
      snprintf(netresizebuff, netresizebuffs, "every %d iteration reload",
               ctx->nth_iteration_data_reload);
    } else {
      snprintf(netresizebuff, netresizebuffs, "newer net resize");
    }
  } else if (ctx->resize_network_every_nth > 0) {
    snprintf(netresizebuff, netresizebuffs, "every %d net resize",
             ctx->resize_network_every_nth);
  }

  CLOGI(
      "%d: %f, %f avg loss, %f rate, %lf seconds, %d images, "
      "%f hours total, %f hours left, %s",
      iteration, ctx->loss, ctx->avg_loss, get_current_rate(ctx->net),
      (what_time_is_it_now() - ctx->time), iteration * ctx->imgs, ctx->avg_time,
      ctx->time_remaining, netresizebuff);
}
