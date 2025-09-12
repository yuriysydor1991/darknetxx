#ifndef DARKNET_CALC_MAP_CTX_FREE_C_FUNCTION_H
#define DARKNET_CALC_MAP_CTX_FREE_C_FUNCTION_H

#include "src/detector/detector_context.h"

#ifdef __cplusplus
extern "C" {
#endif

void calc_map_free_ctx(struct detector_context* ctx);

#ifdef __cplusplus
}
#endif

#endif  // DARKNET_CALC_MAP_CTX_FREE_C_FUNCTION_H