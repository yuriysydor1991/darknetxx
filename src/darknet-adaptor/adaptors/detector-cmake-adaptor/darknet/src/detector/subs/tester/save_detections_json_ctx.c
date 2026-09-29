#include "src/detector/subs/tester/save_detections_json_ctx.h"

#include <assert.h>
#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

#include "memory.h"
#include "src/clog/clog.h"
#include "src/detector/detector_context.h"
#include "utils.h"

int save_detections_json_ctx(struct detector_context *ctx)
{
  // the original test_detector numbered the images starting from the 1
  static const long long int json_image_id = 1;
  static const char *const jsonStart = "[\n";
  static const char *const jsonEnd = "\n]";

  assert(ctx != NULL);

  if (ctx->outfile == NULL) {
    CLOGDM("No detection dst JSON file path provided, skipping");
    return 1;
  }

  char *json_buf = NULL;

  FILE *json_file = fopen(ctx->outfile, "wb");

  if (!json_file) {
    CLOGE("Failed to open the %s JSON file for writing: %s", ctx->outfile,
          strerror(errno));
    return 0;
  }

  fwrite(jsonStart, sizeof(char), strlen(jsonStart), json_file);

  json_buf = detection_to_json(ctx->dets, ctx->nboxes, ctx->l.classes,
                               ctx->names, json_image_id, ctx->input);

  if (json_buf == NULL) {
    CLOGEM("Failed to prepare the detections JSON");
    fclose(json_file);
    return 0;
  }

  fwrite(json_buf, sizeof(char), strlen(json_buf), json_file);

  free(json_buf);

  fwrite(jsonEnd, sizeof(char), strlen(jsonEnd), json_file);

  fclose(json_file);

  return 1;
}
