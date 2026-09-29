#ifndef DARKNETXX_PROJECT_WEIGHTS_FACTORY_NETWORK_CHECKS_H
#define DARKNETXX_PROJECT_WEIGHTS_FACTORY_NETWORK_CHECKS_H

#include <gtest/gtest.h>

#include "WeightsNetSpec.h"
#include "src/darknet.h"
#include "src/network.h"

namespace weights_factory
{

/// @brief The darknet layer type of the generated section kind.
LAYER_TYPE layer_type(LayerKind kind);

/**
 * @brief Checks the darknet network is the one the model describes: the
 * layers types, the outputs and the batch parameters, so the weights file
 * layout the model computes is the network one.
 */
::testing::AssertionResult matches_spec(const network& net, const NetSpec& spec,
                                        ParseMode mode);

}  // namespace weights_factory

#endif  // DARKNETXX_PROJECT_WEIGHTS_FACTORY_NETWORK_CHECKS_H
