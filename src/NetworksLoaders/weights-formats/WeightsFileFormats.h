#ifndef DARKNET_CXX_PORT_PROJECT_WEIGHTS_FILE_FORMATS_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_WEIGHTS_FILE_FORMATS_CLASS_H

#include <string>
#include <vector>

#include "src/NetworksLoaders/IWeightsFileFormat.h"

namespace darknet_adaptor::loaders
{

/// @brief The network weights file formats.
class WeightsFileFormats
{
 public:
  /// @brief Every format in the order of the help messages: the original
  /// weights, the dxxwjz1, the dxxwjz2, the dxxwjz2p, the dxxwjz3 and the
  /// dxxwjz3p one.
  static const std::vector<IWeightsFileFormatPtr>& all();

  /// @brief The formats of the conversion: every one and the YOLO4 folder,
  /// the one of no file name extension of the converter alone.
  static const std::vector<IWeightsFileFormatPtr>& convertible();

  /// @brief The format of the name of the formats, nullptr for an unknown
  /// name.
  static IWeightsFileFormatPtr named(
      const std::string& name,
      const std::vector<IWeightsFileFormatPtr>& formats = all());

  /// @brief The format of the file of its name extension, the original
  /// weights one for the rest.
  static IWeightsFileFormatPtr of_file(const std::string& path);
};

}  // namespace darknet_adaptor::loaders

#endif  // DARKNET_CXX_PORT_PROJECT_WEIGHTS_FILE_FORMATS_CLASS_H
