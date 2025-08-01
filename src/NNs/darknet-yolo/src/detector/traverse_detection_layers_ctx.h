#ifndef DARKNET_CLEAR_CONTEXT_C_FUNCTION_H
#define DARKNET_CLEAR_CONTEXT_C_FUNCTION_H

#include "src/detector/detector_context.h"

#ifdef __cplusplus
extern "C" {
#endif

void traverse_detection_layers_ctx(struct detector_context* ctx);

#ifdef __cplusplus
}
#endif

#endif  // DARKNET_CLEAR_CONTEXT_C_FUNCTION_H