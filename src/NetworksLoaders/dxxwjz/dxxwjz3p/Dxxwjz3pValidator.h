#ifndef DARKNET_CXX_PORT_PROJECT_DXXWJZ3P_V1_VALIDATOR_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_DXXWJZ3P_V1_VALIDATOR_CLASS_H

#include <string>

#include "src/NetworksLoaders/weights-model/WeightsDocument.h"
#include "src/NetworksLoaders/weights-model/WeightsReport.h"
#include "src/darknet.h"

namespace darknet_adaptor::loaders::dxxwjz3p::v1
{

/**
 * @brief Validates the dxxwjz3p files (see the Dxxwjz3pFormat) the way the
 * Dxxwjz2pValidator validates the dxxwjz2p ones.
 */
class Dxxwjz3pValidator
{
 public:
  /// @param net The network of the file, nullptr for the file alone.
  static WeightsReport validate(const std::string& filename,
                                const network* net);

  /// @param document Out: the document of the file.
  static WeightsReport validate(const std::string& filename, const network* net,
                                WeightsDocument& document);
};

}  // namespace darknet_adaptor::loaders::dxxwjz3p::v1

#endif  // DARKNET_CXX_PORT_PROJECT_DXXWJZ3P_V1_VALIDATOR_CLASS_H
