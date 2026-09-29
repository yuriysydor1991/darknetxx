#include "src/detector/subs/tester/load_class_names_ctx.h"

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "data.h"
#include "src/clog/clog.h"

char* get_class_names_file_ctx(const struct detector_context* ctx,
                               const network* net)
{
  assert(ctx != NULL);
  assert(net != NULL);

  if (ctx->name_list != NULL) {
    return ctx->name_list;
  }

  return net->names;
}

int check_class_names_file(const char* names_file)
{
  if (names_file == NULL) {
    CLOGEM(
        "No class names file provided. Provide it with the --names command "
        "line parameter or with the names= key of the cfg [net] section");
    return 0;
  }

  FILE* names_fd = fopen(names_file, "r");

  if (names_fd == NULL) {
    CLOGE(
        "Can't open the class names file %s (%s). Provide it with the --names "
        "command line parameter or with the names= key of the cfg [net] "
        "section",
        names_file, strerror(errno));
    return 0;
  }

  fclose(names_fd);

  return 1;
}

int load_class_names_ctx(struct detector_context* ctx)
{
  assert(ctx != NULL);

  if (ctx == NULL) {
    CLOGEM("Invalid context pointer provided");
    return 0;
  }

  char* names_file = get_class_names_file_ctx(ctx, &ctx->net);

  if (!check_class_names_file(names_file)) {
    return 0;
  }

  ctx->names_size = 0;
  ctx->names = get_labels_custom(names_file, &ctx->names_size);

  const int net_classes = ctx->net.layers[ctx->net.n - 1].classes;

  if (net_classes != ctx->names_size) {
    CLOGE(
        "Error: in the file %s number of names %d that isn't equal to "
        "classes=%d in the file %s",
        names_file, ctx->names_size, net_classes, ctx->cfg);
  }

  // every detected class id indexes the names array
  if (ctx->names_size < ctx->l.classes) {
    CLOGE("The file %s has %d class names for the %d detected classes",
          names_file, ctx->names_size, ctx->l.classes);
    return 0;
  }

  CLOGI("Classes count: %d", ctx->names_size);

  return 1;
}
