#ifndef DARKNET_CXX_PORT_PROJECT_NETWORK_SOURCE_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_NETWORK_SOURCE_CLASS_H

#include <string>

#include "src/NetworksLoaders/weights-model/NetworkDescription.h"
#include "src/NetworksLoaders/weights-model/WeightsDocument.h"

namespace darknet_adaptor::loaders
{

/**
 * @brief The file a network is made of: the network cfg file or the weights
 * file of the format which keeps the network description (the dxxwjz2, the
 * dxxwjz3 and the dxxwjz4 ones and their projects, see the
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

  /**
   * @brief The file the training of the project file saves it's latest
   * weights into: the <name>.dxxwjz4 file beside the <name>.dxxwjz4p
   * project (the project keeps the images, so it isn't rewritten by every
   * save), the project file itself for the rest (the dxxwjz2p and the
   * dxxwjz3p ones).
   */
  static std::string trained_of(const std::string& path);

  /**
   * @brief The weights file the training of the network file resumes from:
   * the trained_of() dxxwjz4 file of the dxxwjz4p project once it exists, the
   * file itself otherwise.
   */
  static std::string resumed_of(const std::string& path);

  /// @brief Tells the project file keeps the images, the dxxwjz4p one.
  static bool keeps_images(const std::string& path);

  /// @brief The value of the key of the [net] section of the network of the
  /// file, the default for none.
  static std::string option_of(const std::string& path, const std::string& key,
                               const std::string& def);
};

}  // namespace darknet_adaptor::loaders

#endif  // DARKNET_CXX_PORT_PROJECT_NETWORK_SOURCE_CLASS_H
