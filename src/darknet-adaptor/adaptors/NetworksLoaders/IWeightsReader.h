#ifndef DARKNET_CXX_PORT_PROJECT_I_WEIGHTS_READER_ABSTRACT_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_I_WEIGHTS_READER_ABSTRACT_CLASS_H

#include <memory>
#include <string>

#include "src/darknet-adaptor/adaptors/ANetworkAdaptor/ANetworkAdaptor.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/weights-model/WeightsDocument.h"
#include "src/darknet.h"

namespace darknet_adaptor::loaders
{

/**
 * @brief The reader of a network weights file format.
 *
 * The reader loads the weights of the file into the network arrays (the
 * load_weights_upto() of the original darknet) and reads the file into the
 * format neutral WeightsDocument, which the writers of every format write,
 * so the files are converted between the formats.
 */
class IWeightsReader
{
 public:
  using IWeightsReaderPtr = std::shared_ptr<IWeightsReader>;

  virtual ~IWeightsReader() = default;
  IWeightsReader() = default;

  /**
   * @brief The load_weights_upto(): loads the weights of the layers before
   * the cutoff from the file into the network arrays, sets the seen and the
   * current iteration of the network.
   *
   * @return False for the file unable to be loaded, where the original ended
   * the process.
   */
  virtual bool load(network& net, const std::string& filename, int cutoff) = 0;

  /// @brief The load_weights(): load() of every layer.
  bool load(network& net, const std::string& filename)
  {
    return load(net, filename, net.n);
  }

  /// @brief The load() of the adaptor's network.
  bool load(adaptors::ANetworkAdaptor& net, const std::string& filename)
  {
    return load(*net.get(), filename);
  }

  /**
   * @brief The load() which keeps the rest of the file: the network
   * description and the training record of the formats which keep them.
   *
   * @param rest Out: the document of the file without its arrays, the empty
   * one for the formats of the weights alone.
   */
  virtual bool load(network& net, const std::string& filename, int cutoff,
                    WeightsDocument& rest)
  {
    rest = WeightsDocument{};
    return load(net, filename, cutoff);
  }

  /**
   * @brief Reads the rest of the file up to its weights: the network
   * description and the training record of the formats which keep them.
   *
   * @param rest Out: the document of the file without its arrays.
   * @param weights Out: tells the file holds any weights.
   *
   * @return False for the file unable to be read and for the formats which
   * keep no network description.
   */
  virtual bool peek(const std::string& filename, WeightsDocument& rest,
                    bool& weights)
  {
    (void)filename;
    rest = WeightsDocument{};
    weights = false;
    return false;
  }

  /**
   * @brief Reads the whole file into the document.
   *
   * @param filename The file to read.
   * @param net The network of the file: the structure of the formats, which
   * keep none, the network the document is validated against for the rest,
   * nullptr for none.
   * @param document Out: the content of the file.
   *
   * @return False for the file unable to be read (or not of the network).
   */
  virtual bool read(const std::string& filename, const network* net,
                    WeightsDocument& document) = 0;
};

using IWeightsReaderPtr = IWeightsReader::IWeightsReaderPtr;

}  // namespace darknet_adaptor::loaders

#endif  // DARKNET_CXX_PORT_PROJECT_I_WEIGHTS_READER_ABSTRACT_CLASS_H
