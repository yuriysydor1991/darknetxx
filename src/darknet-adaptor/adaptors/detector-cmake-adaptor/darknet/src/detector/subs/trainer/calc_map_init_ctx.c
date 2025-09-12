#include "src/detector/subs/trainer/calc_map_init_ctx.h"

#include <assert.h>

#include "data.h"
#include "memory.h"
#include "option_list.h"
#include "parser.h"
#include "src/clog/clog.h"
#include "src/utils.h"

void calc_map_init_ctx(struct detector_context *ctx)
{
  assert(ctx != NULL);

  if (!ctx->calc_map) {
    return;
  }

  FILE *valid_file = fopen(ctx->net.valid_images, "r");
  if (!valid_file) {
    CLOGE(
        "Error: There is no %s file for mAP calculation! Don't use -map "
        "flag. Or set valid=%s in your %s file.",
        ctx->net.valid_images, ctx->net.train_images, ctx->datacfg);
    error("Error!", DARKNET_LOC);
  } else
    fclose(valid_file);

#ifdef GPU
  cuda_set_device(ctx->gpus[0]);
#endif

  CLOGIM(" Prepare additional network for mAP calculation...");
  ctx->net_map = parse_network_cfg_custom(ctx->cfg, 1, 1);
  ctx->net_map.benchmark_layers = ctx->benchmark_layers;
  const int net_classes = ctx->net_map.layers[ctx->net_map.n - 1].classes;

  int k;  // free memory unnecessary arrays
  for (k = 0; k < ctx->net_map.n - 1; ++k)
    free_layer_custom(ctx->net_map.layers[k], 1);

  char *name_list = ctx->net.names;
  int names_size = 0;
  char **names = get_labels_custom(name_list, &names_size);

  if (net_classes != names_size) {
    CLOGE(
        "Error: in the file %s number of names %d that isn't equal to "
        "classes=%d in the file %s",
        name_list, names_size, net_classes, ctx->cfg);
  }

  free_ptrs((void **)names, ctx->net_map.layers[ctx->net_map.n - 1].classes);
}
