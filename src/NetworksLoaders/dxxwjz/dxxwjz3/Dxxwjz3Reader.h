#ifndef DARKNET_CXX_PORT_PROJECT_DXXWJZ3_V3_READER_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_DXXWJZ3_V3_READER_CLASS_H

#include <memory>
#include <string>

#include "src/NetworksLoaders/dxxwjz/dxxwjz2/Dxxwjz2Reader.h"
#include "src/NetworksLoaders/weights-model/WeightsDocument.h"
#include "src/darknet.h"

namespace darknet_adaptor::loaders::dxxwjz3::v3
{

/**
 * @brief The dxxwjz3 file reader (see the Dxxwjz3Format): loads the file the
 * way the Dxxwjz2Reader does, the network description of the cfg text of
 * it's network member.
 */
class Dxxwjz3Reader : public dxxwjz2::v2::Dxxwjz2Reader
{
 public:
  using Dxxwjz3ReaderPtr = std::shared_ptr<Dxxwjz3Reader>;

  virtual ~Dxxwjz3Reader() = default;
  Dxxwjz3Reader() = default;

  static Dxxwjz3ReaderPtr create();

  /// @brief Reads and validates the file (see the Dxxwjz3Validator).
  virtual bool read(const std::string& filename, const network* net,
                    WeightsDocument& document) override;

  virtual bool peek(const std::string& filename, WeightsDocument& rest,
                    bool& weights) override;
};

using Dxxwjz3ReaderPtr = Dxxwjz3Reader::Dxxwjz3ReaderPtr;

}  // namespace darknet_adaptor::loaders::dxxwjz3::v3

#endif  // DARKNET_CXX_PORT_PROJECT_DXXWJZ3_V3_READER_CLASS_H
