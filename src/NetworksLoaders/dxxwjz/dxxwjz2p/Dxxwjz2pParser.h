#ifndef DARKNET_CXX_PORT_PROJECT_DXXWJZ2P_V1_PARSER_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_DXXWJZ2P_V1_PARSER_CLASS_H

#include <filesystem>
#include <istream>
#include <string>

#include "src/NetworksLoaders/weights-model/WeightsDocument.h"
#include "src/NetworksLoaders/weights-model/WeightsReport.h"

namespace darknet_adaptor::loaders::dxxwjz2p::v1
{

/**
 * @brief Reads the JSON text of the dxxwjz2p document (see the Dxxwjz2pFormat)
 * the way the Dxxwjz2Parser reads the dxxwjz2 one, the annotations of the
 * images of the absolute paths of the file ones.
 */
class Dxxwjz2pParser
{
 public:
  /// @param directory The directory of the relative db paths.
  static bool parse(std::istream& json, WeightsDocument& document,
                    WeightsReport& report,
                    const std::filesystem::path& directory = {});

  static bool parse_file(const std::string& filename, WeightsDocument& document,
                         WeightsReport& report);

  /// @brief The Dxxwjz2Parser::peek() of the annotations as well.
  static bool peek(const std::string& filename, WeightsDocument& rest,
                   bool& weights, WeightsReport& report);

  /// @brief The absolute directory of the file, of the relative db paths.
  static std::filesystem::path directory_of(const std::string& filename);
};

}  // namespace darknet_adaptor::loaders::dxxwjz2p::v1

#endif  // DARKNET_CXX_PORT_PROJECT_DXXWJZ2P_V1_PARSER_CLASS_H
