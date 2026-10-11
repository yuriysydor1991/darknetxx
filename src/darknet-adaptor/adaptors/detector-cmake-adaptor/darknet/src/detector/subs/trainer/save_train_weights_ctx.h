#ifndef DARKNET_SAVE_TRAIN_WEIGHTS_CTX_C_FUNCTION_H
#define DARKNET_SAVE_TRAIN_WEIGHTS_CTX_C_FUNCTION_H

#include "src/detector/detector_context.h"

#ifdef __cplusplus
extern "C" {
#endif

/// @brief Saves the weights of the training network (the EMA ones for the
/// ema) into the file of the stem path, see the weights_saver of the context.
void save_train_weights_ctx(struct detector_context* ctx, const char* stem,
                            int ema);

#ifdef __cplusplus
}
#endif

#endif  // DARKNET_SAVE_TRAIN_WEIGHTS_CTX_C_FUNCTION_H
