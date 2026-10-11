#ifndef DARKNET_CXX_PORT_PROJECT_DXXWJZ2_V2_VALIDATOR_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_DXXWJZ2_V2_VALIDATOR_CLASS_H

#include <string>

#include "src/darknet-adaptor/adaptors/NetworksLoaders/weights-model/WeightsDocument.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/weights-model/WeightsReport.h"
#include "src/darknet.h"

namespace darknet_adaptor::loaders::dxxwjz2::v2
{

/**
 * @brief Validates the dxxwjz2 files (see the Dxxwjz2Format) the way the
 * Dxxwjz1Validator validates the dxxwjz1 ones, and their network members: the
 * document without the cfg text is the error, the class names of the count
 * other than the classes of the network are the warning.
 *
 * The document of no weights is the network to train, which keeps the
 * values of the network arrays, so it's validated alone.
 */
class Dxxwjz2Validator
{
 public:
  /// @param net The network of the file, nullptr for the file alone.
  static WeightsReport validate(const std::string& filename,
                                const network* net);

  /// @param document Out: the document of the file.
  static WeightsReport validate(const std::string& filename, const network* net,
                                WeightsDocument& document);

  static WeightsReport validate(const WeightsDocument& document,
                                const network* net);

 private:
  static void check_classes(const WeightsDocument& document, const network& net,
                            WeightsReport& report);
};

}  // namespace darknet_adaptor::loaders::dxxwjz2::v2

#endif  // DARKNET_CXX_PORT_PROJECT_DXXWJZ2_V2_VALIDATOR_CLASS_H
