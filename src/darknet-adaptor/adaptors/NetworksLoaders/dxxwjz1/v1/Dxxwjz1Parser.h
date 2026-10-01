#ifndef DARKNET_CXX_PORT_PROJECT_DXXWJZ1_V1_PARSER_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_DXXWJZ1_V1_PARSER_CLASS_H

#include <istream>
#include <string>

#include "src/darknet-adaptor/adaptors/NetworksLoaders/weights-model/WeightsDocument.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/weights-model/WeightsReport.h"

namespace darknet_adaptor::loaders::dxxwjz1::v1
{

/**
 * @brief Reads the JSON text of the dxxwjz1 document (see the Dxxwjz1Format)
 * into the weights document.
 *
 * The text is parsed by the nlohmann json SAX parser a value at a time, the
 * floats go into the document arrays as they come, so no JSON tree of the
 * document is made (which would take the tens of bytes of every float of the
 * network weights). The structure of the document is checked on the way: the
 * JSON syntax, the format and the version, the types of the values, the
 * names of the layer types, of the sub-layers and of the arrays, the repeated
 * keys, the floats of the float range. The unknown keys are skipped with a
 * warning. The document made is normalized (see WeightsDocument::normalize()),
 * the rest of its checks is the WeightsDocumentValidator one.
 */
class Dxxwjz1Parser
{
 public:
  /**
   * @brief Parses the JSON text of the stream.
   *
   * @param json The text, read up to the end: nothing but the white space
   * may follow the document.
   * @param document Out: the document.
   * @param report Out: the errors and the warnings of the parse.
   *
   * @return False for the text of the errors.
   */
  static bool parse(std::istream& json, WeightsDocument& document,
                    WeightsReport& report);

  /**
   * @brief Parses the dxxwjz1 file: the gzip compressed JSON text, the plain
   * JSON text of the file, which isn't the gzip one, with a warning.
   *
   * @return False for the file unable to be read, the corrupted or the
   * truncated gzip data and the text of the errors.
   */
  static bool parse_file(const std::string& filename, WeightsDocument& document,
                         WeightsReport& report);
};

}  // namespace darknet_adaptor::loaders::dxxwjz1::v1

#endif  // DARKNET_CXX_PORT_PROJECT_DXXWJZ1_V1_PARSER_CLASS_H
