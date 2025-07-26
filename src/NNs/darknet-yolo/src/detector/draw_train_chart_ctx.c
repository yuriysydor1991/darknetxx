#include "src/detector/draw_train_chart_ctx.h"

#include <stdio.h>

#include "image.h"

void draw_train_chart_ctx(struct detector_context* ctx)
{
  assert(ctx != NULL);

  if (ctx == NULL) {
    fprintf(stderr, "detector_args_parser: invalid context provided");
    return;
  }

#ifdef OPENCV
  // int num_threads = get_num_threads();
  // if(num_threads > 2) args.threads = get_num_threads() - 2;
  ctx->args.threads = 6 * ctx->ngpus;  // 3 for - Amazon EC2 Tesla V100: p3.2xlarge
                                  // (8 logical cores) - p3.16xlarge
  // args.threads = 12 * ngpus;    // Ryzen 7 2700X (16 logical cores)

  ctx->img = NULL;
  ctx->max_img_loss = ctx->net.max_chart_loss;
  ctx->number_of_lines = 100;
  ctx->img_size = 1000;

  sprintf(ctx->windows_name, "chart_%s.png", ctx->base);

  ctx->img = draw_train_chart(ctx->windows_name, ctx->max_img_loss, ctx->net.max_batches,
                         ctx->number_of_lines, ctx->img_size, ctx->dont_show,
                         ctx->chart_path);
#endif  // OPENCV
}
