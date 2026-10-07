#ifndef DARKNET_TRAIN_DETECTOR_CTX_C_FUNCTION_H
#define DARKNET_TRAIN_DETECTOR_CTX_C_FUNCTION_H

#include "src/detector/detector_context.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Trains the networks of the context.
 *
 * The networks, loaded by the C++ network infrastructure, have to be the
 * nets views of the context already, with the net_map one for the mAP
 * calculation (see the darknet_adaptor::DarknetContext); they are released
 * by their C++ owner afterwards. The computation resizes and saves them with
 * the darknet C API the C++ implements.
 *
 * @return Zero for the failure.
 */
int train_detector_ctx(struct detector_context* ctx);

#ifdef __cplusplus
}
#endif

#endif  // DARKNET_TRAIN_DETECTOR_CTX_C_FUNCTION_H
