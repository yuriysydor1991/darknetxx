#include "src/detector/calc_map_init_ctx.h"

#include "src/utils.h"
#include "parser.h"

#include <assert.h>

void calc_map_init_ctx(struct detector_context* ctx)
{
  assert(ctx != NULL);
  
  if (!ctx->calc_map) {
    return;
  }

  FILE *valid_file = fopen(ctx->valid_images, "r");
  if (!valid_file) {
    printf(
        "\n Error: There is no %s file for mAP calculation!\n Don't use -map "
        "flag.\n Or set valid=%s in your %s file. \n",
        ctx->valid_images, ctx->train_images, ctx->datacfg);
    error("Error!", DARKNET_LOC);
  } else
    fclose(valid_file);

#ifdef GPU
  cuda_set_device(ctx->gpus[0]);
#endif

  printf(" Prepare additional network for mAP calculation...\n");
  ctx->net_map = parse_network_cfg_custom(ctx->cfg, 1, 1);
  ctx->net_map.benchmark_layers = ctx->benchmark_layers;
  const int net_classes = ctx->net_map.layers[ctx->net_map.n - 1].classes;

  int k;  // free memory unnecessary arrays
  for (k = 0; k < ctx->net_map.n - 1; ++k) free_layer_custom(ctx->net_map.layers[k], 1);

  char *name_list = option_find_str(ctx->options, "names", "data/names.list");
  int names_size = 0;
  char **names = get_labels_custom(name_list, &names_size);
  if (net_classes != names_size) {
    printf(
        "\n Error: in the file %s number of names %d that isn't equal to "
        "classes=%d in the file %s \n",
        name_list, names_size, net_classes, ctx->cfg);
  }
  free_ptrs((void **)names, ctx->net_map.layers[ctx->net_map.n - 1].classes);
}
