#ifndef DARKNET_PRETRAIN_NET_PREPARE_CTX_C_FUNCTION_H
#define DARKNET_PRETRAIN_NET_PREPARE_CTX_C_FUNCTION_H

#include "src/detector/detector_context.h"

#ifdef __cplusplus
extern "C" {
#endif

void pretrain_net_prepare_ctx(struct detector_context* ctx);

#ifdef __cplusplus
}
#endif

#endif  // DARKNET_PRETRAIN_NET_PREPARE_CTX_C_FUNCTION_H