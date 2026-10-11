#ifndef DARKNET_CXX_PORT_PROJECT_DXXWJZ1_V1_VALIDATOR_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_DXXWJZ1_V1_VALIDATOR_CLASS_H

#include <string>

#include "src/NetworksLoaders/weights-model/WeightsDocument.h"
#include "src/NetworksLoaders/weights-model/WeightsReport.h"
#include "src/darknet.h"

namespace darknet_adaptor::loaders::dxxwjz1::v1
{

/**
 * @brief Validates the dxxwjz1 files (see the Dxxwjz1Format).
 *
 * The errors are the files no weights can be loaded of and no file can be
 * converted of: the files unable to be read, the corrupted and the truncated
 * gzip data, the JSON syntax errors, the documents of the other format or of
 * the other version, the documents of the wrong structure (see the
 * Dxxwjz1Parser) and the documents the WeightsDocumentValidator rejects: of
 * their own and, for the network given, of the network.
 *
 * The warnings are the files, which load, but not the way their authors
 * might have expected: the plain JSON files, the unknown keys, the truncated
 * arrays and the network arrays the file doesn't have.
 */
class Dxxwjz1Validator
{
 public:
  /**
   * @brief Validates the file.
   *
   * @param net The network of the file, nullptr for the file alone.
   */
  static WeightsReport validate(const std::string& filename,
                                const network* net);

  /**
   * @brief Validates the file and keeps the document read of it.
   *
   * @param document Out: the document of the file, the empty one for the
   * file unable to be parsed.
   */
  static WeightsReport validate(const std::string& filename, const network* net,
                                WeightsDocument& document);

  /// @brief Validates the document read of a file.
  static WeightsReport validate(const WeightsDocument& document,
                                const network* net);
};

}  // namespace darknet_adaptor::loaders::dxxwjz1::v1

#endif  // DARKNET_CXX_PORT_PROJECT_DXXWJZ1_V1_VALIDATOR_CLASS_H
