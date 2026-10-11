#include "src/detector/subs/trainer/calc_map_init_ctx.h"

#include <assert.h>
#include <stdio.h>

#include "data.h"
#include "src/clog/clog.h"

int calc_map_init_ctx(struct detector_context *ctx)
{
  assert(ctx != NULL);

  if (ctx == NULL) {
    CLOGEM("Invalid context pointer provided");
    return 0;
  }

  if (!ctx->calc_map) {
    return 1;
  }

  // the list of the dxxwjz2p project training is kept in the memory
  const int kept = has_training_data_paths(ctx->net.valid_images);
  FILE *valid_file = kept ? NULL : fopen(ctx->net.valid_images, "r");
  if (!kept && !valid_file) {
    CLOGE(
        "Error: There is no %s file for mAP calculation! Don't use --map "
        "flag. Or set valid=<file> in the [net] section of your %s file.",
        ctx->net.valid_images, ctx->cfg);
    return 0;
  }

  if (valid_file) fclose(valid_file);

  // the mAP calculation network is loaded by the C++ network infrastructure
  // (the class names file of the command line included), it computes with
  // the layers of the training one
  if (ctx->net_map.layers == NULL || ctx->net_map.n <= 0) {
    CLOGEM("No mAP calculation network provided");
    return 0;
  }

  const int net_classes = ctx->net_map.layers[ctx->net_map.n - 1].classes;

  // the class names come from the C++ network infrastructure: the --names
  // file, the classes of the dxxwjz2 file or the names= file of the cfg
  if (ctx->names == NULL || ctx->names_size != net_classes) {
    CLOGE("The mAP calculation requires the names of the %d classes, %d given",
          net_classes, ctx->names_size);
    return 0;
  }

  return 1;
}
