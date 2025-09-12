#ifndef DARKNET_SAVE_WEIGHTS_FINAL_CTX_C_FUNCTION_H
#define DARKNET_SAVE_WEIGHTS_FINAL_CTX_C_FUNCTION_H

#include "src/detector/detector_context.h"

#ifdef __cplusplus
extern "C" {
#endif

void save_weights_final_ctx(struct detector_context* ctx);

#ifdef __cplusplus
}
#endif

#endif  // DARKNET_SAVE_WEIGHTS_CTX_C_FUNCTION_H