#include "src/detector/subs/trainer/draw_train_chart_ctx.h"

#include <stdio.h>

#include "image.h"
#include "src/clog/clog.h"

void draw_train_chart_ctx(struct detector_context* ctx)
{
  assert(ctx != NULL);

  if (ctx == NULL) {
    CLOGEM("invalid context provided");
    return;
  }

#ifdef OPENCV
  ctx->img = NULL;
  ctx->max_img_loss = ctx->net.max_chart_loss;
  ctx->number_of_lines = 100;
  ctx->img_size = 1000;

  sprintf(ctx->windows_name, "chart_%s.png", ctx->base);

  ctx->img = draw_train_chart(ctx->windows_name, ctx->max_img_loss,
                              ctx->net.max_batches, ctx->number_of_lines,
                              ctx->img_size, ctx->dont_show, ctx->chart_path);
#endif  // OPENCV
}
