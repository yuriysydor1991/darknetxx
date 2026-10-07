#include "src/detector/subs/tester/refine_image_filename_ctx.h"

#include <assert.h>

#include "memory.h"
#include "src/clog/clog.h"

int refine_image_filename_ctx(struct detector_context* ctx)
{
  assert(ctx != NULL);

  if (ctx == NULL) {
    CLOGEM("Invalid context pointer provided");
    return 0;
  }

  if (ctx->filename == NULL) {
    CLOGEM("No image filename provided");
    return 0;
  }

  if (ctx->input != NULL) {
    CLOGD("Some filename already provided %s", ctx->input);
    return 1;
  }

  const size_t BUFF_SIZE = strlen(ctx->filename);

  if (BUFF_SIZE == 0) {
    CLOGEM("No image filename provided");
    return 0;
  }

  ctx->input = (char*)xmalloc(BUFF_SIZE + 1);

  memset(ctx->input, 0, BUFF_SIZE + 1);

  strncpy(ctx->input, ctx->filename, BUFF_SIZE);

  const size_t inputSize = strlen(ctx->input);

  if (inputSize > 0 && ctx->input[inputSize - 1] == 0x0d) {
    ctx->input[inputSize - 1] = 0;
  }

  return 1;
}
