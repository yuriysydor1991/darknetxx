#ifndef DARKNET_SAVE_DETECTIONS_LABELS_CTX_C_FUNCTION_H
#define DARKNET_SAVE_DETECTIONS_LABELS_CTX_C_FUNCTION_H

#include "src/detector/detector_context.h"

#ifdef __cplusplus
extern "C" {
#endif

int save_detections_labels_ctx(struct detector_context* ctx);

#ifdef __cplusplus
}
#endif

#endif  // DARKNET_SAVE_DETECTIONS_LABELS_CTX_C_FUNCTION_H
