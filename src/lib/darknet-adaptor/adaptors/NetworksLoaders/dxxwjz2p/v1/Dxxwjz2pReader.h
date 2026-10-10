#ifndef DARKNET_CXX_PORT_PROJECT_DXXWJZ2P_V1_READER_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_DXXWJZ2P_V1_READER_CLASS_H

#include <memory>
#include <string>

#include "src/darknet.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz2/v2/Dxxwjz2Reader.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/weights-model/WeightsDocument.h"

namespace darknet_adaptor::loaders::dxxwjz2p::v1
{

/**
 * @brief The dxxwjz2p file reader (see the Dxxwjz2pFormat): loads the file
 * the way the Dxxwjz2Reader does, the annotations go into the rest of the
 * load() as well.
 */
class Dxxwjz2pReader : public dxxwjz2::v2::Dxxwjz2Reader
{
 public:
  using Dxxwjz2pReaderPtr = std::shared_ptr<Dxxwjz2pReader>;

  virtual ~Dxxwjz2pReader() = default;
  Dxxwjz2pReader() = default;

  static Dxxwjz2pReaderPtr create();

  /// @brief Reads and validates the file (see the Dxxwjz2pValidator).
  virtual bool read(const std::string& filename, const network* net,
                    WeightsDocument& document) override;

  virtual bool peek(const std::string& filename, WeightsDocument& rest,
                    bool& weights) override;
};

using Dxxwjz2pReaderPtr = Dxxwjz2pReader::Dxxwjz2pReaderPtr;

}  // namespace darknet_adaptor::loaders::dxxwjz2p::v1

#endif  // DARKNET_CXX_PORT_PROJECT_DXXWJZ2P_V1_READER_CLASS_H
