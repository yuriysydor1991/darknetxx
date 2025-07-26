#ifndef DARKNET_LOAD_NET_WEIGHTS_CTX_C_FUNCTION_H
#define DARKNET_LOAD_NET_WEIGHTS_CTX_C_FUNCTION_H

#include "src/detector/detector_context.h"

#ifdef __cplusplus
extern "C" {
#endif

void load_net_weights_ctx(struct detector_context* ctx);

#ifdef __cplusplus
}
#endif

#endif  // DARKNET_LOAD_NET_WEIGHTS_CTX_C_FUNCTION_H