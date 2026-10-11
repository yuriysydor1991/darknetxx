#ifndef DARKNET_CXX_PORT_PROJECT_DXXWJZ3P_V1_READER_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_DXXWJZ3P_V1_READER_CLASS_H

#include <memory>
#include <string>

#include "src/NetworksLoaders/dxxwjz/dxxwjz2p/Dxxwjz2pReader.h"
#include "src/NetworksLoaders/weights-model/WeightsDocument.h"
#include "src/darknet.h"

namespace darknet_adaptor::loaders::dxxwjz3p::v1
{

/**
 * @brief The dxxwjz3p file reader (see the Dxxwjz3pFormat): loads the file
 * the way the Dxxwjz2pReader does, the network description of the cfg text
 * of it's network member.
 */
class Dxxwjz3pReader : public dxxwjz2p::v1::Dxxwjz2pReader
{
 public:
  using Dxxwjz3pReaderPtr = std::shared_ptr<Dxxwjz3pReader>;

  virtual ~Dxxwjz3pReader() = default;
  Dxxwjz3pReader() = default;

  static Dxxwjz3pReaderPtr create();

  /// @brief Reads and validates the file (see the Dxxwjz3pValidator).
  virtual bool read(const std::string& filename, const network* net,
                    WeightsDocument& document) override;

  virtual bool peek(const std::string& filename, WeightsDocument& rest,
                    bool& weights) override;
};

using Dxxwjz3pReaderPtr = Dxxwjz3pReader::Dxxwjz3pReaderPtr;

}  // namespace darknet_adaptor::loaders::dxxwjz3p::v1

#endif  // DARKNET_CXX_PORT_PROJECT_DXXWJZ3P_V1_READER_CLASS_H
