#ifndef DARKNET_RUN_DETECTOR_C_FUNCTION_H
#define DARKNET_RUN_DETECTOR_C_FUNCTION_H

#include "src/detector/detector_context.h"

#ifdef __cplusplus
extern "C" {
#endif

int run_detector_ctx(struct detector_context* ctx);

#ifdef __cplusplus
}
#endif

#endif  // DARKNET_RUN_DETECTOR_C_FUNCTION_H