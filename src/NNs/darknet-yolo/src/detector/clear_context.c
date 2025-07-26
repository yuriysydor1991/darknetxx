#include "src/detector/clear_context.h"

#include <assert.h>

#include "image.h"
#include "src/detector/calc_map_free_ctx.h"

void clear_context(struct detector_context* ctx)
{
  assert(ctx != NULL);

  if (ctx == NULL) {
    fprintf(stderr, "detector_args_parser: invalid context provided");
    return 0;
  }

#ifdef OPENCV
  release_mat(&ctx->img);
  destroy_all_windows_cv();
#endif

  // free memory
  pthread_join(load_thread, 0);
  free_data(ctx->buffer);

  free_load_threads(&ctx->args);

  free(ctx->base);
  free(ctx->paths);
  free_list_contents(ctx->plist);
  free_list(ctx->plist);

  free_list_contents_kvp(ctx->options);
  free_list(ctx->options);
  ctx->options = NULL;

  for (int k = 0; k < ctx->ngpus; ++k) free_network(ctx->nets[k]);
  free(ctx->nets);
  ctx->nets = NULL;
  // free_network(net);

  calc_map_free_ctx(ctx);
}
