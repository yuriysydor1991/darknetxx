#ifndef DARKNET_CXX_PORT_PROJECT_DXXWJZ4_V4_WRITER_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_DXXWJZ4_V4_WRITER_CLASS_H

#include <memory>
#include <ostream>
#include <string>

#include "src/NetworksLoaders/dxxwjz/dxxwjz3/Dxxwjz3Writer.h"
#include "src/NetworksLoaders/weights-model/WeightsDocument.h"

namespace darknet_adaptor::loaders::dxxwjz4::v4
{

/**
 * @brief The dxxwjz4 file writer (see the Dxxwjz4Format): writes the file
 * the way the Dxxwjz3Writer does with the description of the rest of the
 * save() and of the document.
 */
class Dxxwjz4Writer : public dxxwjz3::v3::Dxxwjz3Writer
{
 public:
  using Dxxwjz4WriterPtr = std::shared_ptr<Dxxwjz4Writer>;

  virtual ~Dxxwjz4Writer() = default;
  explicit Dxxwjz4Writer(int level = DEFAULT_LEVEL);

  static Dxxwjz4WriterPtr create();

 protected:
  virtual std::unique_ptr<dxxwjz1::v1::Dxxwjz1JsonEmitter> create_emitter(
      std::ostream& out, const WeightsDocument& rest,
      const std::string& filename) const override;
};

using Dxxwjz4WriterPtr = Dxxwjz4Writer::Dxxwjz4WriterPtr;

}  // namespace darknet_adaptor::loaders::dxxwjz4::v4

#endif  // DARKNET_CXX_PORT_PROJECT_DXXWJZ4_V4_WRITER_CLASS_H
