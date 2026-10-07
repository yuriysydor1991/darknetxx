#ifndef DARKNET_PREPARE_DETECTION_CTX_C_FUNCTION_H
#define DARKNET_PREPARE_DETECTION_CTX_C_FUNCTION_H

#include "src/detector/detector_context.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Prepares the context for the detection of the network, which the
 * C++ network infrastructure loaded into the net view (the batch
 * normalization fused into the weights): the detection layers, the class
 * names and the input image.
 *
 * @return Zero for the failure.
 */
int prepare_detection_ctx(struct detector_context* ctx);

#ifdef __cplusplus
}
#endif

#endif  // DARKNET_PREPARE_DETECTION_CTX_C_FUNCTION_H
