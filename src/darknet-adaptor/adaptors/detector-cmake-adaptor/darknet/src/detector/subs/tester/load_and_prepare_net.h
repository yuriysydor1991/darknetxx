#ifndef DARKNET_LOAD_AND_PREPARE_NET_CTX_C_FUNCTION_H
#define DARKNET_LOAD_AND_PREPARE_NET_CTX_C_FUNCTION_H

#include "src/detector/detector_context.h"

#ifdef __cplusplus
extern "C" {
#endif

int load_and_prepare_net(struct detector_context* ctx);

#ifdef __cplusplus
}
#endif

#endif  // DARKNET_LOAD_AND_PREPARE_NET_CTX_C_FUNCTION_H
