#ifndef DARKNET_CXX_PORT_PROJECT_DXXWJZ4_V4_VALIDATOR_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_DXXWJZ4_V4_VALIDATOR_CLASS_H

#include <string>

#include "src/NetworksLoaders/weights-model/WeightsDocument.h"
#include "src/NetworksLoaders/weights-model/WeightsReport.h"
#include "src/darknet.h"

namespace darknet_adaptor::loaders::dxxwjz4::v4
{

/**
 * @brief Validates the dxxwjz4 files (see the Dxxwjz4Format) the way the
 * Dxxwjz3Validator validates the dxxwjz3 ones.
 */
class Dxxwjz4Validator
{
 public:
  /// @param net The network of the file, nullptr for the file alone.
  static WeightsReport validate(const std::string& filename,
                                const network* net);

  /// @param document Out: the document of the file.
  static WeightsReport validate(const std::string& filename, const network* net,
                                WeightsDocument& document);
};

}  // namespace darknet_adaptor::loaders::dxxwjz4::v4

#endif  // DARKNET_CXX_PORT_PROJECT_DXXWJZ4_V4_VALIDATOR_CLASS_H
