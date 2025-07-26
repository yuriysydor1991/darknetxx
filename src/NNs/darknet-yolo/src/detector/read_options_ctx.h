#ifndef DARKNET_READ_NET_OPTIONS_CTX_C_FUNCTION_H
#define DARKNET_READ_NET_OPTIONS_CTX_C_FUNCTION_H

#include "src/detector/detector_context.h"

#ifdef __cplusplus
extern "C" {
#endif

void read_options_ctx(struct detector_context* ctx);

#ifdef __cplusplus
}
#endif

#endif  // DARKNET_READ_NET_OPTIONS_CTX_C_FUNCTION_H