#ifndef DARKNET_DETECT_OBJECTS_CTX_C_FUNCTION_H
#define DARKNET_DETECT_OBJECTS_CTX_C_FUNCTION_H

#include "src/detector/detector_context.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Extracts the detections of the already predicted ctx->im image into
 * the ctx->dets and ctx->nboxes fields and applies the NMS to them.
 *
 * Must be called after the network_predict call and before any of the
 * detections consumers: the JSON and labels savers and the drawing.
 *
 * @return 1 on success, 0 otherwise.
 */
int detect_objects_ctx(struct detector_context* ctx);

#ifdef __cplusplus
}
#endif

#endif  // DARKNET_DETECT_OBJECTS_CTX_C_FUNCTION_H
