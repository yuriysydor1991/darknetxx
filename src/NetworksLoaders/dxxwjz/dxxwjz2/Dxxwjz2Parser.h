#ifndef DARKNET_CXX_PORT_PROJECT_DXXWJZ2_V2_PARSER_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_DXXWJZ2_V2_PARSER_CLASS_H

#include <istream>
#include <string>

#include "src/NetworksLoaders/dxxwjz/dxxwjz1/Dxxwjz1Parser.h"
#include "src/NetworksLoaders/weights-model/WeightsDocument.h"
#include "src/NetworksLoaders/weights-model/WeightsReport.h"

namespace darknet_adaptor::loaders::dxxwjz2::v2
{

/**
 * @brief Reads the JSON text of the dxxwjz2 document (see the Dxxwjz2Format)
 * into the weights document the way the Dxxwjz1Parser reads the dxxwjz1 one:
 * the weights a value at a time, the network and the training members
 * checked on the way.
 */
class Dxxwjz2Parser
{
 public:
  /// @brief Parses the JSON text of the stream, false for the text of the
  /// errors.
  static bool parse(std::istream& json, WeightsDocument& document,
                    WeightsReport& report);

  /// @brief Parses the gzip compressed (or the plain) JSON text of the file.
  static bool parse_file(const std::string& filename, WeightsDocument& document,
                         WeightsReport& report);

  /**
   * @brief Parses the file up to its first weights array.
   *
   * @param rest Out: the document of the file without its arrays.
   * @param weights Out: tells the file holds any weights.
   */
  static bool peek(const std::string& filename, WeightsDocument& rest,
                   bool& weights, WeightsReport& report);

  /// @brief The peek() by the handler of the factory, the one of the peek.
  static bool peek(const std::string& filename,
                   const dxxwjz1::v1::Dxxwjz1Parser::HandlerFactory& factory,
                   WeightsDocument& rest, bool& weights, WeightsReport& report);
};

}  // namespace darknet_adaptor::loaders::dxxwjz2::v2

#endif  // DARKNET_CXX_PORT_PROJECT_DXXWJZ2_V2_PARSER_CLASS_H
