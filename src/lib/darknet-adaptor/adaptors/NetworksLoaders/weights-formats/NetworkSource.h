#ifndef DARKNET_CXX_PORT_PROJECT_NETWORK_SOURCE_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_NETWORK_SOURCE_CLASS_H

#include <string>

#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/weights-model/NetworkDescription.h"

namespace darknet_adaptor::loaders
{

/**
 * @brief The file a network is made of: the network cfg file or the weights
 * file of the format which keeps the network description (the dxxwjz2 one,
 * see the IWeightsFileFormat::keeps_network()).
 */
class NetworkSource
{
 public:
  /// @brief Tells the file keeps the network description beside the
  /// weights.
  static bool describes(const std::string& path);

  /**
   * @brief The description of the network of the file: the cfg text of the
   * cfg file, the description the weights file keeps.
   *
   * @return False for the file unable to be read (logged).
   */
  static bool read(const std::string& path, NetworkDescription& description);

  /**
   * @brief The base name of the files of the training of the network file:
   * the name up to the first dot (the darknet basecfg()), of no backup suffix
   * for the weights file of the network (the _last, _final, _best, _ema and
   * the _<iteration> one of the iteration of its weights).
   */
  static std::string base_of(const std::string& path, int iteration);
};

}  // namespace darknet_adaptor::loaders

#endif  // DARKNET_CXX_PORT_PROJECT_NETWORK_SOURCE_CLASS_H
