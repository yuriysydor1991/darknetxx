#ifndef DARKNET_CXX_PORT_PROJECT_DXXWJZ3P_V1_PARSER_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_DXXWJZ3P_V1_PARSER_CLASS_H

#include <filesystem>
#include <istream>
#include <string>

#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/weights-model/WeightsDocument.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/weights-model/WeightsReport.h"

namespace darknet_adaptor::loaders::dxxwjz3p::v1
{

/**
 * @brief Reads the JSON text of the dxxwjz3p document (see the
 * Dxxwjz3pFormat) the way the Dxxwjz2pParser reads the dxxwjz2p one, by the
 * dxxwjz3p handler.
 */
class Dxxwjz3pParser
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
};

}  // namespace darknet_adaptor::loaders::dxxwjz3p::v1

#endif  // DARKNET_CXX_PORT_PROJECT_DXXWJZ3P_V1_PARSER_CLASS_H
