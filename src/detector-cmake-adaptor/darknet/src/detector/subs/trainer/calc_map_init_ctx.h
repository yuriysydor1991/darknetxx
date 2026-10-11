#ifndef DARKNET_CALC_MAP_INIT_CTX_C_FUNCTION_H
#define DARKNET_CALC_MAP_INIT_CTX_C_FUNCTION_H

#include "src/detector/detector_context.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Checks the ctx->net_map network the C++ network infrastructure
 * loaded for the mAP calculation, when it is requested (the ctx->calc_map
 * flag): the validation images list and the class names of it.
 *
 * Takes the validation images list from the ctx->net network, so must be
 * called after the prepare_train_ctx.
 *
 * @return 1 on success or when no mAP is requested, 0 otherwise.
 */
int calc_map_init_ctx(struct detector_context* ctx);

#ifdef __cplusplus
}
#endif

#endif  // DARKNET_CALC_MAP_INIT_CTX_C_FUNCTION_H
