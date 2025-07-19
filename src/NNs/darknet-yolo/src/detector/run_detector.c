#include "src/detector/run_detector.h"

#include <assert.h>
#include <stdio.h>

#include "src/detector/detector_args_parser.h"
#include "src/detector/detector_context.h"
#include "src/detector/run_detector_ctx.h"

int run_detector(int argc, char **argv)
{
  assert(argc >= 1);
  assert(argv != NULL);

  if (argc <= 0) {
    printf("run_detector: invalid argc value provided\n");
    return 0;
  }

  if (argv == NULL) {
    printf("run_detector: invalid argv pointer provided\n");
    return 0;
  }

  struct detector_context ctx = {0};

  ctx.argc = argc;
  ctx.argv = argv;

  if (!detector_args_parser(&ctx)) {
    printf("run_detector: Failure during detector args parse\n");
    return 0;
  }

  if (!run_detector_ctx(&ctx)) {
    printf("run_detector: Failure while running the detector\n");
    return 0;
  }

  return 1;
}