#ifndef DARKNET_CXX_PORT_PROJECT_ORIG_WEIGHTS_VALIDATOR_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_ORIG_WEIGHTS_VALIDATOR_CLASS_H

#include <cstdint>
#include <string>

#include "src/NetworksLoaders/weights-model/WeightsDocument.h"
#include "src/NetworksLoaders/weights-model/WeightsReport.h"
#include "src/darknet.h"

namespace darknet_adaptor::loaders
{

/// @brief The way the original weights file fits the network.
struct OrigWeightsInspection
{
  WeightsHeader header;
  uint64_t file_size{0U};
  /// the bytes after the header
  uint64_t payload{0U};
  /// the floats of the arrays the save_weights() writes of the network
  uint64_t saved_floats{0U};
  /// the floats of the arrays the load_weights() reads of the network: no
  /// dontload layers, no batch normalization arrays of the dontloadscales
  /// ones
  uint64_t loaded_floats{0U};
  /**
   * @brief The file holds the arrays the load_weights() reads rather than
   * the ones the save_weights() writes: the payload is of the size of the
   * former and not of the latter one (the file made for the network of the
   * dontload and the dontloadscales layers).
   */
  bool loading_layout{false};

  /// @brief The floats of the arrays the file holds.
  uint64_t expected_floats() const;
};

/**
 * @brief Validates the original darknet weights file of the network.
 *
 * The file keeps no structure of its own: the header and the float arrays of
 * the network one after another. What tells the file of the network from the
 * file of another one (or the complete file from the truncated one) is it's
 * size, the validator compares it with the arrays of the network.
 *
 * The errors are the files no weights can be loaded of: the ones unable to be
 * read, the ones shorter than the header and the gzip compressed ones (the
 * dxxwjz1 files, for example). The warnings are the files the
 * darknet loads partly or ignores the part of: the truncated files, the files
 * of the part of a float, the files longer than the network arrays.
 */
class OrigWeightsValidator
{
 public:
  /**
   * @brief Reads the header and the size of the file.
   *
   * @return False for the file unable to open or shorter than the header.
   */
  static bool inspect(const std::string& filename, const network& net,
                      OrigWeightsInspection& inspection);

  /// @brief Validates the file of the network.
  static WeightsReport validate(const std::string& filename,
                                const network& net);

  /// @brief Validates the inspected file.
  static WeightsReport validate(const OrigWeightsInspection& inspection);

  /**
   * @brief Validates the document to be written as the original weights file
   * of the network.
   *
   * The file keeps the arrays one after another with no names of them, so
   * the arrays of the document have to be the arrays of the network from the
   * first one on (the ones the save_weights() writes or the ones the
   * load_weights() reads), all of them complete but the last one (the
   * truncated file). The array the document misses in the middle or the
   * shorter one followed by others (the document edited by hand, for example)
   * is the error: the loaders would read the arrays after it into the wrong
   * arrays of the network.
   */
  static WeightsReport validate(const WeightsDocument& document,
                                const network& net);
};

}  // namespace darknet_adaptor::loaders

#endif  // DARKNET_CXX_PORT_PROJECT_ORIG_WEIGHTS_VALIDATOR_CLASS_H
