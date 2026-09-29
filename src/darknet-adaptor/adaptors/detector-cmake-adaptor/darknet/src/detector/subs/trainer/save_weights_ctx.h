#ifndef DARKNET_SAVE_WEIGHTS_CTX_C_FUNCTION_H
#define DARKNET_SAVE_WEIGHTS_CTX_C_FUNCTION_H

#include "src/detector/detector_context.h"

#ifdef __cplusplus
extern "C" {
#endif

/// @brief Saves the periodic checkpoints of the current training iteration.
void save_weights_ctx(struct detector_context* ctx);

/// @brief Saves the current weights as the <base>_last.weights file (and the
/// <base>_ema.weights one when the EMA is used).
void save_weights_last_ctx(struct detector_context* ctx);

#ifdef __cplusplus
}
#endif

#endif  // DARKNET_SAVE_WEIGHTS_CTX_C_FUNCTION_H
