#include "src/detector/subs/tester/show_detections_ctx.h"

#include <assert.h>
#include <limits.h>
#include <stdlib.h>

#include "image.h"
#include "image_opencv.h"
#include "src/clog/clog.h"
#include "src/detector/detector_context.h"

static const char* const PREDICTIONS = "predictions";

// Waits for a key press in the predictions window, but not after the window is
// closed and not after the computation is asked to stop, so a stop request
// isn't held by the shown window.
static void wait_for_key_or_stop(struct detector_context* ctx)
{
  static const int POLL_MS = 100;

  // the first poll gets the window shown
  if (wait_key_cv(POLL_MS) >= 0) {
    return;
  }

  // A highgui backend which doesn't report the window visibility can't tell a
  // closed window apart, so it gets the original blocking wait for a key.
  if (is_window_visible_cv(PREDICTIONS) < 0) {
    wait_until_press_key_cv();
    return;
  }

  while (!detector_context_is_stopped(ctx) &&
         is_window_visible_cv(PREDICTIONS) > 0) {
    if (wait_key_cv(POLL_MS) >= 0) {
      return;
    }
  }
}

int show_detections_ctx(struct detector_context* ctx)
{
  assert(ctx != NULL);

  if (ctx == NULL) {
    CLOGEM("Invalid context pointer provided");
    return 0;
  }

  ctx->alphabet = load_alphabet();

  // An image without detections is a regular result too, so it gets saved
  // (and shown) the same way to never leave a stale predictions image behind.
  draw_detections_v3(ctx->im, ctx->dets, ctx->nboxes, ctx->thresh, ctx->names,
                     ctx->alphabet, ctx->l.classes, ctx->ext_output,
                     ctx->names_size);

  save_image(ctx->im, PREDICTIONS);

  if (!ctx->dont_show) {
    show_image(ctx->im, PREDICTIONS);
    wait_for_key_or_stop(ctx);
    destroy_all_windows_cv();
  }

  free_alphabet(ctx->alphabet);
  ctx->alphabet = NULL;

  return 1;
}
