#ifndef DARKNET_LOAD_CLASS_NAMES_CTX_C_FUNCTION_H
#define DARKNET_LOAD_CLASS_NAMES_CTX_C_FUNCTION_H

#include "src/detector/detector_context.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Checks the class names of the ctx->names and ctx->names_size fields,
 * which the C++ network infrastructure gives the detection, against the
 * ctx->net network.
 *
 * Must be called after the network is parsed and before anything uses the
 * names, so the missing names stop the detection before any output is
 * written.
 *
 * @return 1 on success, 0 otherwise.
 */
int load_class_names_ctx(struct detector_context* ctx);

#ifdef __cplusplus
}
#endif

#endif  // DARKNET_LOAD_CLASS_NAMES_CTX_C_FUNCTION_H
