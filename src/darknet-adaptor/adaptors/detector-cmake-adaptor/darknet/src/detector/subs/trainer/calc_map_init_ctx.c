#include "src/detector/subs/trainer/calc_map_init_ctx.h"

#include <assert.h>

#include "data.h"
#include "option_list.h"
#include "src/clog/clog.h"
#include "src/detector/subs/tester/load_class_names_ctx.h"
#include "src/utils.h"

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

  FILE *valid_file = fopen(ctx->net.valid_images, "r");
  if (!valid_file) {
    CLOGE(
        "Error: There is no %s file for mAP calculation! Don't use --map "
        "flag. Or set valid=<file> in the [net] section of your %s file.",
        ctx->net.valid_images, ctx->cfg);
    return 0;
  }

  fclose(valid_file);

  // the mAP calculation network is loaded by the C++ network infrastructure
  // (the class names file of the command line included), it computes with
  // the layers of the training one
  if (ctx->net_map.layers == NULL || ctx->net_map.n <= 0) {
    CLOGEM("No mAP calculation network provided");
    return 0;
  }

  const int net_classes = ctx->net_map.layers[ctx->net_map.n - 1].classes;

  char *name_list = get_class_names_file_ctx(ctx, &ctx->net_map);

  if (!check_class_names_file(name_list)) {
    return 0;
  }

  int names_size = 0;
  char **names = get_labels_custom(name_list, &names_size);

  if (net_classes != names_size) {
    CLOGE(
        "Error: in the file %s number of names %d that isn't equal to "
        "classes=%d in the file %s",
        name_list, names_size, net_classes, ctx->cfg);
  }

  free_ptrs((void **)names, names_size);

  return 1;
}
