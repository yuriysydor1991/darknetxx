#ifndef DARKNET_CXX_PORT_PROJECT_WEIGHTS_DOCUMENT_VALIDATOR_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_WEIGHTS_DOCUMENT_VALIDATOR_CLASS_H

#include "src/darknet-adaptor/adaptors/NetworksLoaders/weights-model/WeightsDocument.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/weights-model/WeightsReport.h"
#include "src/darknet.h"

namespace darknet_adaptor::loaders
{

/**
 * @brief Validates the format neutral weights documents, the ones every
 * weights format reads and writes (see the WeightsDocument).
 *
 * The errors are the documents no network can be loaded from or no file can
 * be written of: the 32 bits seen header of the seen beyond the 32 bits, the
 * repeated layers, the arrays the layers of their types don't have and, for
 * the network, the layers of the other types, the arrays the network doesn't
 * have and the arrays longer than the network ones.
 *
 * The warnings are the documents, which load partly: the arrays shorter than
 * the network ones and the network arrays the document doesn't have (the
 * truncated files, the files saved up to a layer), which keep the network
 * values.
 */
class WeightsDocumentValidator
{
 public:
  /// @brief The document alone: the header, the layers and the arrays of
  /// their types.
  static WeightsReport validate(const WeightsDocument& document);

  /// @brief The document of the network: the validate() of the document
  /// alone and the layers, the arrays and the sizes of the network.
  static WeightsReport validate(const WeightsDocument& document,
                                const network& net);
};

}  // namespace darknet_adaptor::loaders

#endif  // DARKNET_CXX_PORT_PROJECT_WEIGHTS_DOCUMENT_VALIDATOR_CLASS_H
