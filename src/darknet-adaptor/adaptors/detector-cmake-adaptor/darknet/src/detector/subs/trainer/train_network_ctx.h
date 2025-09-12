#ifndef DARKNET_TRAIN_NETWORK_CTX_C_FUNCTION_H
#define DARKNET_TRAIN_NETWORK_CTX_C_FUNCTION_H

#include "src/detector/detector_context.h"

#ifdef __cplusplus
extern "C" {
#endif

void train_network_ctx(struct detector_context* ctx);

#ifdef __cplusplus
}
#endif

#endif  // DARKNET_TRAIN_NETWORK_CTX_C_FUNCTION_H