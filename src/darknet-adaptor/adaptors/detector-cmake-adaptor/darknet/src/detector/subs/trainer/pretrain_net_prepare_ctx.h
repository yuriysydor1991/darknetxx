#ifndef DARKNET_PRETRAIN_NET_PREPARE_CTX_C_FUNCTION_H
#define DARKNET_PRETRAIN_NET_PREPARE_CTX_C_FUNCTION_H

#include "src/detector/detector_context.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Resizes the network every Nth iteration of a random=1 network (see
 * the --net-resize-nth and the --dont-resize-network options) and prepares the
 * loader arguments for the new input size.
 *
 * @return 1 if the network is resized, so it needs a batch of the new input
 * size, and 0 otherwise.
 */
int pretrain_net_prepare_ctx(struct detector_context* ctx);

#ifdef __cplusplus
}
#endif

#endif  // DARKNET_PRETRAIN_NET_PREPARE_CTX_C_FUNCTION_H
