#ifndef DARKNET_RECOMPUTE_AVG_LOSS_CTX_C_FUNCTION_H
#define DARKNET_RECOMPUTE_AVG_LOSS_CTX_C_FUNCTION_H

#include "src/detector/detector_context.h"

#ifdef __cplusplus
extern "C" {
#endif

void recompute_avg_loss_ctx(struct detector_context* ctx);

#ifdef __cplusplus
}
#endif

#endif  // DARKNET_RECOMPUTE_AVG_LOSS_CTX_C_FUNCTION_H