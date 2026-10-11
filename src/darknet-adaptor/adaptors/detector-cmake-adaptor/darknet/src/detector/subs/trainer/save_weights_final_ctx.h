#ifndef DARKNET_SAVE_WEIGHTS_FINAL_CTX_C_FUNCTION_H
#define DARKNET_SAVE_WEIGHTS_FINAL_CTX_C_FUNCTION_H

#include "src/detector/detector_context.h"

#ifdef __cplusplus
extern "C" {
#endif

/// @brief Saves the result of the finished training as the <base>_final
/// file, see the save_train_weights_ctx.
void save_weights_final_ctx(struct detector_context* ctx);

#ifdef __cplusplus
}
#endif

#endif  // DARKNET_SAVE_WEIGHTS_CTX_C_FUNCTION_H
