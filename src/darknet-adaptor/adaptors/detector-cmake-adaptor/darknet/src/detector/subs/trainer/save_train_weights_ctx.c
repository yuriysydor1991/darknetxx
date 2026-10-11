#include "src/detector/subs/trainer/save_train_weights_ctx.h"

#include <assert.h>
#include <limits.h>
#include <stdio.h>

#include "parser.h"
#include "src/clog/clog.h"
#include "utils.h"

void save_train_weights_ctx(struct detector_context* ctx, const char* stem,
                            const int ema)
{
  assert(ctx != NULL);
  assert(stem != NULL);

  if (ctx->weights_saver != NULL) {
    if (!ctx->weights_saver(ctx, stem, ema, ctx->weights_saver_data)) {
      // the original save_weights() ended the process the same way
      file_error(__FILE__, __LINE__, stem);
    }

    return;
  }

  char path[PATH_MAX];

  snprintf(path, sizeof(path), "%s.weights", stem);

  save_weights_upto(ctx->net, path, ctx->net.n, ema);
}
