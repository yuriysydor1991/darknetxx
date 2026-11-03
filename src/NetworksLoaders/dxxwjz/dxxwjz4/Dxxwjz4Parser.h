#ifndef DARKNET_CXX_PORT_PROJECT_DXXWJZ4_V4_PARSER_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_DXXWJZ4_V4_PARSER_CLASS_H

#include <istream>
#include <string>

#include "src/NetworksLoaders/weights-model/WeightsDocument.h"
#include "src/NetworksLoaders/weights-model/WeightsReport.h"

namespace darknet_adaptor::loaders::dxxwjz4::v4
{

/**
 * @brief Reads the JSON text of the dxxwjz4 document (see the Dxxwjz4Format)
 * the way the Dxxwjz3Parser reads the dxxwjz3 one, by the dxxwjz4 handler.
 */
class Dxxwjz4Parser
{
 public:
  static bool parse(std::istream& json, WeightsDocument& document,
                    WeightsReport& report);

  static bool parse_file(const std::string& filename, WeightsDocument& document,
                         WeightsReport& report);

  /// @brief The Dxxwjz3Parser::peek() of the description as well.
  static bool peek(const std::string& filename, WeightsDocument& rest,
                   bool& weights, WeightsReport& report);
};

}  // namespace darknet_adaptor::loaders::dxxwjz4::v4

#endif  // DARKNET_CXX_PORT_PROJECT_DXXWJZ4_V4_PARSER_CLASS_H
