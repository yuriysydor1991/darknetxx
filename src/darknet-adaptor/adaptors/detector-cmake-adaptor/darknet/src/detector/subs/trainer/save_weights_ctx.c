#include "src/detector/subs/trainer/save_weights_ctx.h"

#include <assert.h>
#include <limits.h>
#include <stdlib.h>

#include "parser.h"
#include "src/clog/clog.h"
#include "src/detector/subs/trainer/sync_gpu_nets_ctx.h"

void save_weights_ctx(struct detector_context* ctx)
{
  static const size_t BUFF_SIZE = PATH_MAX;

  assert(ctx != NULL);

  if (ctx == NULL) {
    CLOGEM("invalid train context pointer provided");
    return;
  }

  const int iteration = ctx->current_iteration;

  if ((iteration >= (ctx->iter_save + ctx->net.saveweights) ||
       iteration % ctx->net.saveweights == 0)) {
    ctx->iter_save = iteration;
    sync_gpu_nets_ctx(ctx);
    char buff[BUFF_SIZE];
    memset(buff, 0, BUFF_SIZE);
    sprintf(buff, "%s/%s_%d.weights", ctx->net.backup_directory, ctx->base,
            iteration);
    save_weights(ctx->net, buff);
  }

  if ((ctx->net.saveweights > ctx->net.savelast) &&
      (iteration >= (ctx->iter_save_last + ctx->net.savelast) ||
       (iteration % ctx->net.savelast == 0 && iteration > 1))) {
    ctx->iter_save_last = iteration;
    sync_gpu_nets_ctx(ctx);
    char buff[BUFF_SIZE];
    memset(buff, 0, BUFF_SIZE);
    sprintf(buff, "%s/%s_last.weights", ctx->net.backup_directory, ctx->base);
    save_weights(ctx->net, buff);

    if (ctx->net.ema_alpha && is_ema_initialized(ctx->net)) {
      sprintf(buff, "%s/%s_ema.weights", ctx->net.backup_directory, ctx->base);
      save_weights_upto(ctx->net, buff, ctx->net.n, 1);
      CLOGI(" EMA weights are saved to the file: %s", buff);
    }
  }
}
