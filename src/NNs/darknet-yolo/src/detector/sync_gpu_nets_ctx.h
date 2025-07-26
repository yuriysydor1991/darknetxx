#ifndef DARKNET_SYNC_GPU_NETS_CTX_C_FUNCTION_H
#define DARKNET_SYNC_GPU_NETS_CTX_C_FUNCTION_H

#include "src/detector/detector_context.h"

#ifdef __cplusplus
extern "C" {
#endif

void sync_gpu_nets_ctx(struct detector_context* ctx);

#ifdef __cplusplus
}
#endif

#endif  // DARKNET_SYNC_GPU_NETS_CTX_C_FUNCTION_H