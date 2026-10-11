#ifndef DARKNET_CXX_PORT_PROJECT_NETWORK_SOURCE_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_NETWORK_SOURCE_CLASS_H

#include <string>

#include "src/darknet-adaptor/adaptors/NetworksLoaders/weights-model/NetworkDescription.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/weights-model/WeightsDocument.h"

namespace darknet_adaptor::loaders
{

/**
 * @brief The file a network is made of: the network cfg file or the weights
 * file of the format which keeps the network description (the dxxwjz2 and
 * the dxxwjz3 ones and their projects, see the
 * IWeightsFileFormat::keeps_network()).
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
   * @brief The read() of everything the file keeps but the weights: the
   * network description, the training record and the YOLO4 annotations of
   * the project files, the cfg text of the cfg file.
   */
  static bool read(const std::string& path, WeightsDocument& rest);

  /**
   * @brief The base name of the files of the training of the network file:
   * the name up to the first dot (the darknet basecfg()), of no backup suffix
   * for the weights file of the network (the _last, _final, _best, _ema and
   * the _<iteration>_<avg loss> one, or the _<iteration> one of the earlier
   * trainings, of the iteration of its weights).
   */
  static std::string base_of(const std::string& path, int iteration);

  /// @brief The value of the key of the [net] section of the network of the
  /// file, the default for none.
  static std::string option_of(const std::string& path, const std::string& key,
                               const std::string& def);
};

}  // namespace darknet_adaptor::loaders

#endif  // DARKNET_CXX_PORT_PROJECT_NETWORK_SOURCE_CLASS_H
