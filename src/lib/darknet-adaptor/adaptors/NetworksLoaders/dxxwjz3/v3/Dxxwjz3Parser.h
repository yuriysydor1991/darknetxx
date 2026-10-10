#ifndef DARKNET_CXX_PORT_PROJECT_DXXWJZ3_V3_PARSER_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_DXXWJZ3_V3_PARSER_CLASS_H

#include <istream>
#include <string>

#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/weights-model/WeightsDocument.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/weights-model/WeightsReport.h"

namespace darknet_adaptor::loaders::dxxwjz3::v3
{

/**
 * @brief Reads the JSON text of the dxxwjz3 document (see the Dxxwjz3Format)
 * the way the Dxxwjz2Parser reads the dxxwjz2 one, by the dxxwjz3 handler.
 */
class Dxxwjz3Parser
{
 public:
  static bool parse(std::istream& json, WeightsDocument& document,
                    WeightsReport& report);

  static bool parse_file(const std::string& filename, WeightsDocument& document,
                         WeightsReport& report);

  /// @brief The Dxxwjz2Parser::peek() of the dxxwjz3 file.
  static bool peek(const std::string& filename, WeightsDocument& rest,
                   bool& weights, WeightsReport& report);
};

}  // namespace darknet_adaptor::loaders::dxxwjz3::v3

#endif  // DARKNET_CXX_PORT_PROJECT_DXXWJZ3_V3_PARSER_CLASS_H
