#include "src/detector/print_net_stat_ctx.h"

#include <assert.h>
#include <stdio.h>

#include "utils.h"

void print_net_stat_ctx(struct detector_context* ctx)
{
  const int iteration = get_current_iteration(ctx->net);
    // i = get_current_batch(net);

  int calc_map_for_each =
      ctx->mAP_epochs * ctx->train_images_num /
      (ctx->net.batch * ctx->net.subdivisions);  // calculate mAP every mAP_epochs
  calc_map_for_each = fmax(calc_map_for_each, 100);
  ctx->next_map_calc = ctx->iter_map + calc_map_for_each;
  ctx->next_map_calc = fmax(ctx->next_map_calc, ctx->net.burn_in);
  // next_map_calc = fmax(next_map_calc, 400);

  if (ctx->calc_map) {
    printf("\n (next mAP calculation at %d iterations) ", ctx->next_map_calc);
    if (ctx->mean_average_precision > 0)
      printf("\n Last accuracy mAP@%0.2f = %2.2f %%, best = %2.2f %% ",
              ctx->iou_thresh, ctx->mean_average_precision * 100, ctx->best_map * 100);
  }

  printf("\033[H\033[J");
  
  if (ctx->mean_average_precision > 0.0) {
    printf("%d/%d: loss=%0.1f map=%0.2f best=%0.2f hours left=%0.1f\007",
            iteration, ctx->net.max_batches, ctx->loss, ctx->mean_average_precision, ctx->best_map,
            ctx->avg_time);
  } else {
    printf("%d/%d: loss=%0.1f hours left=%0.1f\007", iteration,
            ctx->net.max_batches, ctx->loss, ctx->avg_time);
  }

  if (ctx->net.cudnn_half) {
    if (iteration < ctx->net.burn_in * 3)
      fprintf(stderr,
              "\n Tensor Cores are disabled until the first %d iterations "
              "are reached.\n",
              3 * ctx->net.burn_in);
    else
      fprintf(stderr, "\n Tensor Cores are used.\n");

    fflush(stderr);
  }

  printf(
      "\n %d: %f, %f avg loss, %f rate, %lf seconds, %d images, %f hours "
      "left\n",
      iteration, ctx->loss, ctx->avg_loss, get_current_rate(ctx->net),
      (what_time_is_it_now() - ctx->time), iteration * ctx->imgs, ctx->avg_time);

  fflush(stdout);
}
