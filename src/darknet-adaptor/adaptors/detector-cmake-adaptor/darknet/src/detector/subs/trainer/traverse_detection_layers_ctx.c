#include "src/detector/subs/trainer/traverse_detection_layers_ctx.h"

#include <assert.h>
#include <stdio.h>

#include "src/clog/clog.h"

void traverse_detection_layers_ctx(struct detector_context* ctx)
{
  assert(ctx != NULL);

  if (ctx == NULL) {
    CLOGEM("detector_args_parser: invalid context provided");
    return;
  }

  ctx->l = ctx->net.layers[ctx->net.n - 1];

  for (int k = 0; k < ctx->net.n; ++k) {
    layer lk = ctx->net.layers[k];
    if (lk.type == YOLO || lk.type == GAUSSIAN_YOLO || lk.type == REGION) {
      ctx->l = lk;
      CLOGI(" Detection layer: %d - type = %d", k, ctx->l.type, 0);
    }
  }

  ctx->classes = ctx->l.classes;
}
