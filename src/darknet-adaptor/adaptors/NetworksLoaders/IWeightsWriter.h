#ifndef DARKNET_CXX_PORT_PROJECT_I_WEIGHTS_WRITER_ABSTRACT_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_I_WEIGHTS_WRITER_ABSTRACT_CLASS_H

#include <memory>
#include <string>

#include "src/darknet-adaptor/adaptors/ANetworkAdaptor/ANetworkAdaptor.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/weights-model/WeightsDocument.h"
#include "src/darknet.h"

namespace darknet_adaptor::loaders
{

/**
 * @brief The writer of a network weights file format.
 *
 * The writer saves the weights of the network arrays into the file (the
 * save_weights_upto() of the original darknet) and writes the format neutral
 * WeightsDocument, which the readers of every format read, so the files are
 * converted between the formats.
 */
class IWeightsWriter
{
 public:
  using IWeightsWriterPtr = std::shared_ptr<IWeightsWriter>;

  virtual ~IWeightsWriter() = default;
  IWeightsWriter() = default;

  /**
   * @brief The save_weights_upto(): saves the weights of the layers before
   * the cutoff, the EMA ones of the convolutional layers for the save_ema,
   * with the seen of the current iteration (which updates the seen of the
   * network).
   *
   * @return False for the file unable to be written, where the original
   * ended the process.
   */
  virtual bool save(network& net, const std::string& filename, int cutoff,
                    bool save_ema) = 0;

  /// @brief The save_weights(): save() of every layer.
  bool save(network& net, const std::string& filename)
  {
    return save(net, filename, net.n, false);
  }

  /// @brief The save() of the adaptor's network.
  bool save(adaptors::ANetworkAdaptor& net, const std::string& filename)
  {
    return save(*net.get(), filename);
  }

  /**
   * @brief The save() with the rest of the file: the network description and
   * the training record of the document, which the formats of the weights
   * alone leave out.
   */
  virtual bool save(network& net, const std::string& filename, int cutoff,
                    bool save_ema, const WeightsDocument& rest)
  {
    (void)rest;
    return save(net, filename, cutoff, save_ema);
  }

  /**
   * @brief Writes the document into the file.
   *
   * @return False for the file unable to be written.
   */
  virtual bool write(const WeightsDocument& document,
                     const std::string& filename) = 0;
};

using IWeightsWriterPtr = IWeightsWriter::IWeightsWriterPtr;

}  // namespace darknet_adaptor::loaders

#endif  // DARKNET_CXX_PORT_PROJECT_I_WEIGHTS_WRITER_ABSTRACT_CLASS_H
