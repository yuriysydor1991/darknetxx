#ifndef DARKNET_CXX_PORT_PROJECT_DXXWJZ3_V3_VALIDATOR_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_DXXWJZ3_V3_VALIDATOR_CLASS_H

#include <string>

#include "src/darknet-adaptor/adaptors/NetworksLoaders/weights-model/WeightsDocument.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/weights-model/WeightsReport.h"
#include "src/darknet.h"

namespace darknet_adaptor::loaders::dxxwjz3::v3
{

/**
 * @brief Validates the dxxwjz3 files (see the Dxxwjz3Format) the way the
 * Dxxwjz2Validator validates the dxxwjz2 ones, the JSON of the network cfg
 * and of the trailing bytes is checked by the parse.
 */
class Dxxwjz3Validator
{
 public:
  /// @param net The network of the file, nullptr for the file alone.
  static WeightsReport validate(const std::string& filename,
                                const network* net);

  /// @param document Out: the document of the file.
  static WeightsReport validate(const std::string& filename, const network* net,
                                WeightsDocument& document);
};

}  // namespace darknet_adaptor::loaders::dxxwjz3::v3

#endif  // DARKNET_CXX_PORT_PROJECT_DXXWJZ3_V3_VALIDATOR_CLASS_H
