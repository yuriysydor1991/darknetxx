#ifndef DARKNET_SHOW_DETECTIONS_CTX_C_FUNCTION_H
#define DARKNET_SHOW_DETECTIONS_CTX_C_FUNCTION_H

#include "src/detector/detector_context.h"

#ifdef __cplusplus
extern "C" {
#endif

int show_detections_ctx(struct detector_context* ctx);

#ifdef __cplusplus
}
#endif

#endif  // DARKNET_SHOW_DETECTIONS_CTX_C_FUNCTION_H