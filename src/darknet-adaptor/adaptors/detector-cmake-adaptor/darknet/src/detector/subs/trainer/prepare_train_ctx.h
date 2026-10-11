#ifndef DARKNET_PREPARE_TRAIN_CTX_C_FUNCTION_H
#define DARKNET_PREPARE_TRAIN_CTX_C_FUNCTION_H

#include "src/detector/detector_context.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Prepares the context for the training of the networks, which the
 * C++ network infrastructure loaded into the nets views (one per GPU): the
 * iteration counters, the batch sizes and the training images list.
 *
 * @return Zero for the failure (no networks, no training images).
 */
int prepare_train_ctx(struct detector_context* ctx);

#ifdef __cplusplus
}
#endif

#endif  // DARKNET_PREPARE_TRAIN_CTX_C_FUNCTION_H
