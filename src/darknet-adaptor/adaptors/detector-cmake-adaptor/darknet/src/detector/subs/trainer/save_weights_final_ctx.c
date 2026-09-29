#include "src/detector/subs/trainer/save_weights_final_ctx.h"

#include <assert.h>
#include <limits.h>
#include <stdlib.h>

#include "parser.h"
#include "src/clog/clog.h"
#include "src/detector/subs/trainer/sync_gpu_nets_ctx.h"

void save_weights_final_ctx(struct detector_context* ctx)
{
  static const size_t BUFF_SIZE = PATH_MAX;

  assert(ctx != NULL);

  char buff[BUFF_SIZE];

  sprintf(buff, "%s/%s_final.weights", ctx->net.backup_directory, ctx->base);

  save_weights(ctx->net, buff);
}
