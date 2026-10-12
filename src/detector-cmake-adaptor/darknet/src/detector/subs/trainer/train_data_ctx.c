#include "src/detector/subs/trainer/train_data_ctx.h"

#include <assert.h>
#include <pthread.h>
#include <string.h>

#include "data.h"
#include "src/clog/clog.h"

int train_data_reload_due_ctx(const struct detector_context* ctx,
                              const int iteration)
{
  assert(ctx != NULL);

  if (ctx->nth_iteration_data_reload > 0) {
    return iteration % ctx->nth_iteration_data_reload == 0;
  }

  return ctx->reload_data;
}

void start_train_data_load_ctx(struct detector_context* ctx)
{
  assert(ctx != NULL);

  if (ctx->data_loading) {
    return;
  }

  ctx->load_thread = load_data(ctx->args);
  ctx->data_loading = 1;
}

static void finish_train_data_load(struct detector_context* ctx)
{
  pthread_join(ctx->load_thread, 0);
  ctx->data_loading = 0;
}

void take_train_data_ctx(struct detector_context* ctx)
{
  assert(ctx != NULL);

  CLOGDM("Taking a new training batch");

  start_train_data_load_ctx(ctx);
  finish_train_data_load(ctx);

  free_data(ctx->train);
  ctx->train = ctx->buffer;

  // the classifier trains the classes of the boxes (see the
  // prepare_load_args_ctx()), the next load replaces the buffer anyway
  if (ctx->l.type == SOFTMAX) {
    const matrix boxes = ctx->train.y;

    ctx->train.y = classes_of_boxes(boxes, ctx->args.truth_size, ctx->classes);
    free_matrix(boxes);
  }
}

void drop_train_data_load_ctx(struct detector_context* ctx)
{
  assert(ctx != NULL);

  if (!ctx->data_loading) {
    return;
  }

  finish_train_data_load(ctx);

  // the loader has put the new batch there, while the ctx->train keeps its own
  free_data(ctx->buffer);
  memset(&ctx->buffer, 0, sizeof(ctx->buffer));
}

void free_train_data_ctx(struct detector_context* ctx)
{
  assert(ctx != NULL);

  drop_train_data_load_ctx(ctx);

  free_data(ctx->train);
  memset(&ctx->train, 0, sizeof(ctx->train));
  memset(&ctx->buffer, 0, sizeof(ctx->buffer));
}
