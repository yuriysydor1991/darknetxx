#include "src/detector/traverse_detection_layers_ctx.h"

#include <stdio.h>
#include <assert.h>

void traverse_detection_layers_ctx(struct detector_context* ctx)
{
  assert(ctx != NULL);

  if (ctx == NULL) {
    fprintf(stderr, "detector_args_parser: invalid context provided");
    return 0;
  }

  ctx->l = ctx->net.layers[ctx->net.n - 1];

  for (int k = 0; k < ctx->net.n; ++k) {
    layer lk = ctx->net.layers[k];
    if (lk.type == YOLO || lk.type == GAUSSIAN_YOLO || lk.type == REGION) {
      ctx->l = lk;
      printf(" Detection layer: %d - type = %d \n", k, ctx->l.type);
    }
  }

  ctx->classes = ctx->l.classes;
}