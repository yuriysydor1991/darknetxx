#ifndef DARKNET_CXX_PORT_PROJECT_DXXWJZ1_V1_READER_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_DXXWJZ1_V1_READER_CLASS_H

#include <memory>
#include <string>

#include "src/NetworksLoaders/IWeightsReader.h"
#include "src/NetworksLoaders/weights-model/WeightsDocument.h"
#include "src/darknet.h"

namespace darknet_adaptor::loaders::dxxwjz1::v1
{

/**
 * @brief The dxxwjz1 file reader (see the Dxxwjz1Format).
 *
 * The file is validated before the network is touched (see the
 * Dxxwjz1Validator): the file of the errors, the file of another network
 * included, is loaded into no network. The load follows the one of the
 * original weights file: the seen and the current iteration of the header,
 * the transposed connected weights of the major or minor > 1000 versions, no
 * dontload layers, no batch normalization arrays of the dontloadscales
 * layers, the flipped convolutional weights transposed. The arrays are found
 * by their layers and their names rather than by the position in the file,
 * so the skipped arrays of a layer shift no arrays of the others.
 *
 * The array shorter than the network one fills its beginning, the absent
 * arrays keep the network values (the documents of the truncated files and
 * of the files saved up to a layer).
 *
 * The readers of the later versions read and validate their files by their
 * read().
 */
class Dxxwjz1Reader : public IWeightsReader
{
 public:
  using Dxxwjz1ReaderPtr = std::shared_ptr<Dxxwjz1Reader>;
  using IWeightsReader::load;

  virtual ~Dxxwjz1Reader() = default;
  Dxxwjz1Reader() = default;

  static Dxxwjz1ReaderPtr create();

  /**
   * @brief The load_weights_upto() of the dxxwjz1 file: loads the arrays of
   * the layers before the cutoff, sets the seen and the current iteration of
   * the network.
   *
   * @return False for the file of the errors, the network stays as it is.
   */
  virtual bool load(network& net, const std::string& filename,
                    int cutoff) override;

  /// @brief The load() which keeps the document of the file without its
  /// arrays: the header and the rest the later versions keep.
  virtual bool load(network& net, const std::string& filename, int cutoff,
                    WeightsDocument& rest) override;

  /**
   * @brief Reads and validates the file: the file alone for no network, the
   * file of the network otherwise.
   *
   * @return False for the file of the errors.
   */
  virtual bool read(const std::string& filename, const network* net,
                    WeightsDocument& document) override;

  /**
   * @brief Loads the arrays of the validated document of the network.
   *
   * @return The count of the layers loaded.
   */
  static int apply(const WeightsDocument& document, network& net, int cutoff);

 protected:
  /// @brief Tells the read document is the network to train, the load()
  /// applies nothing of (the later versions), false here.
  virtual bool network_to_train(WeightsDocument& document) const;
};

using Dxxwjz1ReaderPtr = Dxxwjz1Reader::Dxxwjz1ReaderPtr;

}  // namespace darknet_adaptor::loaders::dxxwjz1::v1

#endif  // DARKNET_CXX_PORT_PROJECT_DXXWJZ1_V1_READER_CLASS_H
