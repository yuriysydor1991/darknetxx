#ifndef DARKNET_CALC_MAP_INIT_CTX_C_FUNCTION_H
#define DARKNET_CALC_MAP_INIT_CTX_C_FUNCTION_H

#include "src/detector/detector_context.h"

#ifdef __cplusplus
extern "C" {
#endif

void prepare_load_args_ctx(struct detector_context* ctx);

#ifdef __cplusplus
}
#endif

#endif  // DARKNET_CALC_MAP_INIT_CTX_C_FUNCTION_H