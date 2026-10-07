#include "src/detector/subs/tester/save_detections_labels_ctx.h"

#include <assert.h>
#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

#include "src/clog/clog.h"
#include "src/detector/detector_context.h"
#include "utils.h"

int save_detections_labels_ctx(struct detector_context* ctx)
{
  static const size_t BUFF_SIZE = PATH_MAX;

  assert(ctx != NULL);

  // pseudo labeling concept - fast.ai
  if (!ctx->save_labels) {
    return 1;
  }

  char labelpath[BUFF_SIZE];

  memset(labelpath, 0, BUFF_SIZE);

  replace_image_to_label(ctx->input, labelpath);

  FILE* fw = fopen(labelpath, "wb");

  if (fw == NULL) {
    CLOGE("Failed to open the %s labels file for writing: %s", labelpath,
          strerror(errno));
    return 0;
  }

  for (int i = 0; i < ctx->nboxes; ++i) {
    int class_id = -1;
    float prob = 0;

    for (int j = 0; j < ctx->l.classes; ++j) {
      if (ctx->dets[i].prob[j] > ctx->thresh && ctx->dets[i].prob[j] > prob) {
        prob = ctx->dets[i].prob[j];
        class_id = j;
      }
    }

    if (class_id >= 0) {
      char buff[BUFF_SIZE];
      sprintf(buff, "%d %2.4f %2.4f %2.4f %2.4f\n", class_id,
              ctx->dets[i].bbox.x, ctx->dets[i].bbox.y, ctx->dets[i].bbox.w,
              ctx->dets[i].bbox.h);
      fwrite(buff, sizeof(char), strlen(buff), fw);
    }
  }

  fclose(fw);

  return 1;
}
