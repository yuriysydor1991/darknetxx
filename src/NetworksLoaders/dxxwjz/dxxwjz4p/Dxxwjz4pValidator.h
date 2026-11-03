#ifndef DARKNET_CXX_PORT_PROJECT_DXXWJZ4P_V1_VALIDATOR_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_DXXWJZ4P_V1_VALIDATOR_CLASS_H

#include <string>

#include "src/NetworksLoaders/weights-model/WeightsDocument.h"
#include "src/NetworksLoaders/weights-model/WeightsReport.h"
#include "src/darknet.h"

namespace darknet_adaptor::loaders::dxxwjz4p::v1
{

/**
 * @brief Validates the dxxwjz4p files (see the Dxxwjz4pFormat) the way the
 * Dxxwjz3pValidator validates the dxxwjz3p ones, the dxxwjz4p archive
 * unpacked (see the Dxxwjz4pParser).
 */
class Dxxwjz4pValidator
{
 public:
  /// @param net The network of the file, nullptr for the file alone.
  static WeightsReport validate(const std::string& filename,
                                const network* net);

  /// @param document Out: the document of the file.
  static WeightsReport validate(const std::string& filename, const network* net,
                                WeightsDocument& document);
};

}  // namespace darknet_adaptor::loaders::dxxwjz4p::v1

#endif  // DARKNET_CXX_PORT_PROJECT_DXXWJZ4P_V1_VALIDATOR_CLASS_H
