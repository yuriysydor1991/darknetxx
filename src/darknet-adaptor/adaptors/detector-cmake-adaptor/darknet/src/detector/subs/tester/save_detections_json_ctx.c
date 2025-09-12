#include "src/detector/subs/tester/save_detections_json_ctx.h"

#include <assert.h>
#include <limits.h>
#include <stdlib.h>

#include "memory.h"
#include "src/detector/detector_context.h"
#include "utils.h"

int save_detections_json_ctx(struct detector_context *ctx)
{
  static const size_t BUFF_SIZE = PATH_MAX;
  static const char *const jsonStart = "[\n";
  static const char *const jsonEnd = "\n]";

  if (ctx->outfile == NULL) {
    printf("No detection dst JSON file path provided, skipping\n");
    return 1;
  }

  char *json_buf = NULL;

  FILE *json_file = fopen(ctx->outfile, "wb");

  if (!json_file) {
    error("fopen failed", DARKNET_LOC);
    return 0;
  }

  fwrite(jsonStart, sizeof(char), strlen(jsonStart), json_file);

  json_buf = detection_to_json(ctx->dets, ctx->nboxes, ctx->l.classes,
                               ctx->names, 0, ctx->input);

  assert(json_buf != NULL);

  fwrite(json_buf, sizeof(char), strlen(json_buf), json_file);

  free(json_buf);

  fwrite(jsonEnd, sizeof(char), strlen(jsonEnd), json_file);

  fclose(json_file);

  return 1;
}
