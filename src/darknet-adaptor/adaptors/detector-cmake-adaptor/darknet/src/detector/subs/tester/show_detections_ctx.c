#include "src/detector/subs/tester/show_detections_ctx.h"

#include <assert.h>
#include <limits.h>
#include <stdlib.h>

#include "box.h"
#include "data.h"
#include "image.h"
#include "src/clog/clog.h"
#include "src/detector/detector_context.h"

int show_detections_ctx(struct detector_context* ctx)
{
  static const float nms = .45f;  // 0.4F

  ctx->nboxes = 0;
  ctx->dets =
      get_network_boxes(&ctx->net, ctx->im.w, ctx->im.h, ctx->thresh,
                        ctx->hier_thresh, 0, 1, &ctx->nboxes, ctx->letter_box);

  if (ctx->nboxes <= 0) {
    CLOGIM("NO DETECTIONS!");
    return 0;
  }

  int selected_detections_num = 0;
  detection_with_class* selected_detections = get_actual_detections(
      ctx->dets, ctx->nboxes, ctx->thresh, &selected_detections_num, ctx->names,
      ctx->names_size);

  if (selected_detections_num <= 0) {
    CLOGIM("NO DETECTIONS!");
    free(selected_detections);
    return 0;
  }

  if (ctx->net.names == NULL) {
    CLOGEM("No class names file path");
    free(selected_detections);
    return 0;
  }

  ctx->names_size = 0;

  ctx->names = get_labels_custom(ctx->net.names, &ctx->names_size);

  if (ctx->net.layers[ctx->net.n - 1].classes != ctx->names_size) {
    CLOGE(
        "Error: in the file %s number of names %d that isn't equal to "
        "classes=%d in the file %s",
        ctx->net.names, ctx->names_size,
        ctx->net.layers[ctx->net.n - 1].classes, ctx->cfg);
  }

  CLOGI("Classes count: %d", ctx->names_size);

  ctx->alphabet = load_alphabet();

  CLOGI("Detected %d classes", ctx->nboxes);

  if (nms) {
    if (ctx->l.nms_kind == DEFAULT_NMS)
      do_nms_sort(ctx->dets, ctx->nboxes, ctx->l.classes, nms);
    else
      diounms_sort(ctx->dets, ctx->nboxes, ctx->l.classes, nms, ctx->l.nms_kind,
                   ctx->l.beta_nms);
  }

  draw_detections_v3(ctx->im, ctx->dets, ctx->nboxes, ctx->thresh, ctx->names,
                     ctx->alphabet, ctx->l.classes, ctx->ext_output,
                     ctx->names_size);

  save_image(ctx->im, "predictions");

  if (!ctx->dont_show) {
    show_image(ctx->im, "predictions");
    wait_until_press_key_cv();
    destroy_all_windows_cv();
  }

  free_detections(ctx->dets, ctx->nboxes);
  free_image(ctx->im);
  free_image(ctx->sized);
  free_alphabet(ctx->alphabet);
  free(selected_detections);

  return 1;
}
