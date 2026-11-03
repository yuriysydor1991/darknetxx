#ifndef DARKNET_CXX_PORT_PROJECT_DXXWJZ4_V4_READER_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_DXXWJZ4_V4_READER_CLASS_H

#include <memory>
#include <string>

#include "src/NetworksLoaders/dxxwjz/dxxwjz3/Dxxwjz3Reader.h"
#include "src/NetworksLoaders/weights-model/WeightsDocument.h"
#include "src/darknet.h"

namespace darknet_adaptor::loaders::dxxwjz4::v4
{

/**
 * @brief The dxxwjz4 file reader (see the Dxxwjz4Format): loads the file the
 * way the Dxxwjz3Reader does, the description goes into the rest of the
 * load() as well.
 */
class Dxxwjz4Reader : public dxxwjz3::v3::Dxxwjz3Reader
{
 public:
  using Dxxwjz4ReaderPtr = std::shared_ptr<Dxxwjz4Reader>;

  virtual ~Dxxwjz4Reader() = default;
  Dxxwjz4Reader() = default;

  static Dxxwjz4ReaderPtr create();

  /// @brief Reads and validates the file (see the Dxxwjz4Validator).
  virtual bool read(const std::string& filename, const network* net,
                    WeightsDocument& document) override;

  virtual bool peek(const std::string& filename, WeightsDocument& rest,
                    bool& weights) override;
};

using Dxxwjz4ReaderPtr = Dxxwjz4Reader::Dxxwjz4ReaderPtr;

}  // namespace darknet_adaptor::loaders::dxxwjz4::v4

#endif  // DARKNET_CXX_PORT_PROJECT_DXXWJZ4_V4_READER_CLASS_H
