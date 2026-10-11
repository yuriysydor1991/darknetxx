#ifndef DARKNET_CXX_PORT_PROJECT_WEIGHTS_CONVERSION_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_WEIGHTS_CONVERSION_CLASS_H

#include <functional>
#include <string>

#include "src/darknet-adaptor/adaptors/NetworksLoaders/IWeightsFileFormat.h"

namespace darknet_adaptor::loaders
{

/// @brief One conversion of a network weights file.
class WeightsConversion
{
 public:
  virtual ~WeightsConversion() = default;
  WeightsConversion() = default;

  /// the format of the file to read, nullptr with no file to read for the
  /// network to train of the cfg (the dxxwjz2 file of no weights)
  IWeightsFileFormatPtr from;
  /// the file to read, empty for the network to train of the cfg
  std::string src;
  IWeightsFileFormatPtr to;
  /// the file to write, replaced by the complete file alone
  std::string dst;
  /**
   * @brief The network cfg file of the weights: required to read the formats
   * of no structure of their own, the network the rest is validated against
   * and the cfg the formats of the network description keep. Empty for
   * none.
   */
  std::string cfg;
  /// @brief The class names file the formats of the network description
  /// keep, empty for none.
  std::string names;
  /// @brief Asked between the steps of the conversion, true stops it (the
  /// stop request of the application, for example). Empty for none.
  std::function<bool()> stop_requested;

  /// @brief Tells the stop is requested, which is logged.
  bool stopped() const;
};

}  // namespace darknet_adaptor::loaders

#endif  // DARKNET_CXX_PORT_PROJECT_WEIGHTS_CONVERSION_CLASS_H
