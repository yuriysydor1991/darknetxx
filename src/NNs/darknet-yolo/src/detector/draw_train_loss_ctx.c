#include "src/detector/draw_train_loss_ctx.h"

#include <assert.h>
#include <stdlib.h>

#include "image_opencv.h"

void draw_train_loss_ctx(struct detector_context* ctx)
{
#ifdef OPENCV
  if (ctx->net.contrastive) {
    float cur_con_acc = -1;
    for (int k = 0; k < ctx->net.n; ++k)
      if (ctx->net.layers[k].type == CONTRASTIVE)
        cur_con_acc = *ctx->net.layers[k].loss;
    if (cur_con_acc >= 0)
      ctx->avg_contrastive_acc = ctx->avg_contrastive_acc * 0.99 + cur_con_acc * 0.01;
    printf("  avg_contrastive_acc = %f \n", ctx->avg_contrastive_acc);
  }

  const int iteration = get_current_iteration(ctx->net);

  draw_train_loss(ctx->windows_name, ctx->img, ctx->img_size, ctx->avg_loss, ctx->max_img_loss,
                  iteration, ctx->net.max_batches, ctx->mean_average_precision,
                  ctx->draw_precision, "mAP%", ctx->avg_contrastive_acc / 100,
                  ctx->dont_show, ctx->mjpeg_port, ctx->avg_time);
#endif  // OPENCV
}