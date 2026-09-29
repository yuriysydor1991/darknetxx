#ifndef DARKNET_TEST_DETECTOR_CTX_C_FUNCTION_H
#define DARKNET_TEST_DETECTOR_CTX_C_FUNCTION_H

#include "src/detector/detector_context.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Detects the objects of the context image and writes the requested
 * outputs of them.
 *
 * The network, loaded by the C++ network infrastructure, has to be the net
 * view of the context already (see the darknet_adaptor::DarknetContext); it
 * is released by it's C++ owner afterwards.
 *
 * @return Zero for the failure.
 */
int test_detector_ctx(struct detector_context* ctx);

#ifdef __cplusplus
}
#endif

#endif  // DARKNET_TEST_DETECTOR_CTX_C_FUNCTION_H
