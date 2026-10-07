#ifndef DARKNET_CXX_PORT_PROJECT_DXXWJZ2_V2_READER_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_DXXWJZ2_V2_READER_CLASS_H

#include <memory>
#include <string>

#include "src/darknet.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz1/v1/Dxxwjz1Reader.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/weights-model/WeightsDocument.h"

namespace darknet_adaptor::loaders::dxxwjz2::v2
{

/**
 * @brief The dxxwjz2 file reader (see the Dxxwjz2Format): loads the weights
 * the way the Dxxwjz1Reader does, the network description and the training
 * record go into the rest of the load(). The file of no weights loads none,
 * the network keeps its values; the one of the network alone (of no layers)
 * keeps it's seen and no record either.
 */
class Dxxwjz2Reader : public dxxwjz1::v1::Dxxwjz1Reader
{
 public:
  using Dxxwjz2ReaderPtr = std::shared_ptr<Dxxwjz2Reader>;
  using Dxxwjz1Reader::load;

  virtual ~Dxxwjz2Reader() = default;
  Dxxwjz2Reader() = default;

  static Dxxwjz2ReaderPtr create();

  /// @brief Reads and validates the file (see the Dxxwjz2Validator).
  virtual bool read(const std::string& filename, const network* net,
                    WeightsDocument& document) override;

  virtual bool peek(const std::string& filename, WeightsDocument& rest,
                    bool& weights) override;

 protected:
  /// @brief The network alone, which trains from scratch: neither its header
  /// nor its training record is of the network.
  virtual bool network_to_train(WeightsDocument& document) const override;
};

using Dxxwjz2ReaderPtr = Dxxwjz2Reader::Dxxwjz2ReaderPtr;

}  // namespace darknet_adaptor::loaders::dxxwjz2::v2

#endif  // DARKNET_CXX_PORT_PROJECT_DXXWJZ2_V2_READER_CLASS_H
