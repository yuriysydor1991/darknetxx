#ifndef DARKNET_CXX_PORT_PROJECT_DXXWJZ4P_V1_READER_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_DXXWJZ4P_V1_READER_CLASS_H

#include <memory>
#include <string>

#include "src/NetworksLoaders/dxxwjz/dxxwjz3p/Dxxwjz3pReader.h"
#include "src/NetworksLoaders/weights-model/WeightsDocument.h"
#include "src/darknet.h"

namespace darknet_adaptor::loaders::dxxwjz4p::v1
{

/**
 * @brief The dxxwjz4p project reader (see the Dxxwjz4pFormat): loads the
 * project the way the Dxxwjz3pReader does of the JSON document and the images
 * of the archive unpacked into the temporary folder (see the
 * Dxxwjz4pWorkspace), the description goes into the rest of the load() as
 * well.
 */
class Dxxwjz4pReader : public dxxwjz3p::v1::Dxxwjz3pReader
{
 public:
  using Dxxwjz4pReaderPtr = std::shared_ptr<Dxxwjz4pReader>;

  virtual ~Dxxwjz4pReader() = default;
  Dxxwjz4pReader() = default;

  static Dxxwjz4pReaderPtr create();

  /// @brief Reads and validates the file (see the Dxxwjz4pValidator).
  virtual bool read(const std::string& filename, const network* net,
                    WeightsDocument& document) override;

  virtual bool peek(const std::string& filename, WeightsDocument& rest,
                    bool& weights) override;
};

using Dxxwjz4pReaderPtr = Dxxwjz4pReader::Dxxwjz4pReaderPtr;

}  // namespace darknet_adaptor::loaders::dxxwjz4p::v1

#endif  // DARKNET_CXX_PORT_PROJECT_DXXWJZ4P_V1_READER_CLASS_H
