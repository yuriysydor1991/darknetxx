#include "src/detector/subs/trainer/clear_context.h"

#include <assert.h>

#include "image.h"
#include "src/clog/clog.h"
#include "src/detector/subs/trainer/calc_map_free_ctx.h"
#include "src/detector/subs/trainer/train_data_ctx.h"

void clear_context(struct detector_context* ctx)
{
  assert(ctx != NULL);

  if (ctx == NULL) {
    CLOGEM("invalid context pointer provided");
    return;
  }

#ifdef OPENCV
  destroy_all_windows_cv();
  release_mat(&ctx->img);
#endif

  // free memory (waits for a batch loading in the background first)
  free_train_data_ctx(ctx);

  free_load_threads(&ctx->args);

  free(ctx->base);
  free(ctx->paths);
  free_list_contents(ctx->plist);
  free_list(ctx->plist);

  // the networks belong to the C++ network infrastructure, which releases
  // them after the training

  calc_map_free_ctx(ctx);
}
