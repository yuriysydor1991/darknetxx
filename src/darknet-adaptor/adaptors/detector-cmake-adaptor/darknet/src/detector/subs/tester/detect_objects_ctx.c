#include "src/detector/subs/tester/detect_objects_ctx.h"

#include <assert.h>

#include "box.h"
#include "network.h"
#include "src/clog/clog.h"

int detect_objects_ctx(struct detector_context* ctx)
{
  static const float nms = .45f;  // 0.4F

  assert(ctx != NULL);

  if (ctx == NULL) {
    CLOGEM("Invalid context pointer provided");
    return 0;
  }

  ctx->nboxes = 0;
  ctx->dets =
      get_network_boxes(&ctx->net, ctx->im.w, ctx->im.h, ctx->thresh,
                        ctx->hier_thresh, 0, 1, &ctx->nboxes, ctx->letter_box);

  CLOGD("Found %d candidate boxes", ctx->nboxes);

  if (nms) {
    if (ctx->l.nms_kind == DEFAULT_NMS)
      do_nms_sort(ctx->dets, ctx->nboxes, ctx->l.classes, nms);
    else
      diounms_sort(ctx->dets, ctx->nboxes, ctx->l.classes, nms, ctx->l.nms_kind,
                   ctx->l.beta_nms);
  }

  return 1;
}
