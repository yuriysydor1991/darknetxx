#include "src/detector/subs/trainer/draw_train_chart_ctx.h"

#include <stdio.h>
#include <string.h>
#include <time.h>

#include "image.h"
#include "src/clog/clog.h"

#ifdef OPENCV
// the date and time of the start of the training, which tells the charts of
// the resumed trainings apart
static void training_start_text(char* text, const size_t size)
{
  const time_t now = time(NULL);
  struct tm local;

#ifdef _WIN32
  localtime_s(&local, &now);
#else
  localtime_r(&now, &local);
#endif

  strftime(text, size, "%Y-%m-%d--%H-%M-%S", &local);
}
#endif  // OPENCV

void draw_train_chart_ctx(struct detector_context* ctx)
{
  assert(ctx != NULL);

  if (ctx == NULL) {
    CLOGEM("invalid context provided");
    return;
  }

#ifdef OPENCV
  ctx->img = NULL;
  ctx->max_img_loss = ctx->net.max_chart_loss;
  ctx->number_of_lines = 100;
  ctx->img_size = 1000;

  char started[32];

  training_start_text(started, sizeof(started));
  snprintf(ctx->windows_name, sizeof(ctx->windows_name), "chart_%s_%s.png",
           ctx->base, started);

  CLOGI("The training chart: %s", ctx->windows_name);

  // the record of the resumed training redraws the whole chart
  const int recorded = ctx->resumed.points > 0 || ctx->resumed.map_points > 0;

  if (recorded && ctx->chart_path != NULL) {
    CLOGI("The training record redraws the chart, the %s image isn't continued",
          ctx->chart_path);
  }

  ctx->img = draw_train_chart(ctx->windows_name, ctx->max_img_loss,
                              ctx->net.max_batches, ctx->number_of_lines,
                              ctx->img_size, ctx->dont_show,
                              recorded ? NULL : ctx->chart_path);

  memset(&ctx->chart_state, 0, sizeof(ctx->chart_state));
  draw_train_record(ctx->img, ctx->img_size, ctx->max_img_loss,
                    ctx->net.max_batches, &ctx->resumed, &ctx->chart_state);
  ctx->chart_state.saved_batch = ctx->current_iteration;
  memset(&ctx->resumed, 0, sizeof(ctx->resumed));

  save_train_chart_ctx(ctx);
#endif  // OPENCV
}

void save_train_chart_ctx(struct detector_context* ctx)
{
#ifdef OPENCV
  if (ctx->img != NULL) {
    save_cv_png(ctx->img, ctx->windows_name);
  }
#else
  (void)ctx;
#endif  // OPENCV
}
