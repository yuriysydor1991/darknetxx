#include "src/detector/subs/tester/load_class_names_ctx.h"

#include <assert.h>

#include "src/clog/clog.h"

int load_class_names_ctx(struct detector_context* ctx)
{
  assert(ctx != NULL);

  if (ctx == NULL) {
    CLOGEM("Invalid context pointer provided");
    return 0;
  }

  // the class names come from the C++ network infrastructure, which keeps
  // them: the --names file, the classes of the dxxwjz2 file or the names=
  // file of the network cfg
  if (ctx->names == NULL || ctx->names_size <= 0) {
    CLOGEM(
        "No class names of the network. Provide them with the --names command "
        "line parameter, the dxxwjz2 weights file of the classes or the "
        "names= key of the cfg [net] section");
    return 0;
  }

  const int net_classes = ctx->net.layers[ctx->net.n - 1].classes;

  if (net_classes != ctx->names_size) {
    CLOGE("Error: the %d class names aren't the classes=%d of the network %s",
          ctx->names_size, net_classes, ctx->cfg);
  }

  // every detected class id indexes the names array
  if (ctx->names_size < ctx->l.classes) {
    CLOGE("The %d class names are less than the %d detected classes",
          ctx->names_size, ctx->l.classes);
    return 0;
  }

  CLOGI("Classes count: %d", ctx->names_size);

  return 1;
}
