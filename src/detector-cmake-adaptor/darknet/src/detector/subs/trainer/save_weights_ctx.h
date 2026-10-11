#ifndef DARKNET_SAVE_WEIGHTS_CTX_C_FUNCTION_H
#define DARKNET_SAVE_WEIGHTS_CTX_C_FUNCTION_H

#include "src/detector/detector_context.h"

#ifdef __cplusplus
extern "C" {
#endif

/// @brief Saves the periodic checkpoints of the current training iteration:
/// the numbered <base>_<iteration>_<avg loss> file and the <base>_last one.
void save_weights_ctx(struct detector_context* ctx);

/// @brief Saves the current weights as the <base>_last file (and the
/// <base>_ema one when the EMA is used), see the save_train_weights_ctx.
void save_weights_last_ctx(struct detector_context* ctx);

#ifdef __cplusplus
}
#endif

#endif  // DARKNET_SAVE_WEIGHTS_CTX_C_FUNCTION_H
