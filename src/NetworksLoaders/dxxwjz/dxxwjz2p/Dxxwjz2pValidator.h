#ifndef DARKNET_CXX_PORT_PROJECT_DXXWJZ2P_V1_VALIDATOR_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_DXXWJZ2P_V1_VALIDATOR_CLASS_H

#include <string>

#include "src/NetworksLoaders/weights-model/WeightsDocument.h"
#include "src/NetworksLoaders/weights-model/WeightsReport.h"
#include "src/darknet.h"

namespace darknet_adaptor::loaders::dxxwjz2p::v1
{

/**
 * @brief Validates the dxxwjz2p files (see the Dxxwjz2pFormat) the way the
 * Dxxwjz2Validator validates the dxxwjz2 ones, the annotations are checked
 * by the parse: the boxes of the classes of the network, of the images of
 * the size.
 */
class Dxxwjz2pValidator
{
 public:
  /// @param net The network of the file, nullptr for the file alone.
  static WeightsReport validate(const std::string& filename,
                                const network* net);

  /// @param document Out: the document of the file.
  static WeightsReport validate(const std::string& filename, const network* net,
                                WeightsDocument& document);
};

}  // namespace darknet_adaptor::loaders::dxxwjz2p::v1

#endif  // DARKNET_CXX_PORT_PROJECT_DXXWJZ2P_V1_VALIDATOR_CLASS_H
