#include "src/detector/subs/trainer/clear_context.h"

#include <assert.h>

#include "image.h"
#include "src/clog/clog.h"
#include "src/detector/subs/trainer/calc_map_free_ctx.h"

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

  // free memory
  // pthread_join(ctx->load_thread, 0);
  free_data(ctx->buffer);

  free_load_threads(&ctx->args);

  free(ctx->base);
  free(ctx->paths);
  free_list_contents(ctx->plist);
  free_list(ctx->plist);

  for (int k = 0; k < ctx->ngpus; ++k) free_network(ctx->nets[k]);
  free(ctx->nets);
  ctx->nets = NULL;

  calc_map_free_ctx(ctx);
}
