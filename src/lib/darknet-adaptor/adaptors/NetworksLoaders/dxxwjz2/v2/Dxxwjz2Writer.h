#ifndef DARKNET_CXX_PORT_PROJECT_DXXWJZ2_V2_WRITER_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_DXXWJZ2_V2_WRITER_CLASS_H

#include <memory>
#include <ostream>
#include <string>

#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz1/v1/Dxxwjz1Writer.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/weights-model/WeightsDocument.h"

namespace darknet_adaptor::loaders::dxxwjz2::v2
{

/**
 * @brief The dxxwjz2 file writer (see the Dxxwjz2Format): writes the weights
 * the way the Dxxwjz1Writer does with the network description and the
 * training record of the rest of the save() and of the document. The file
 * keeps the network description, so the rest without the cfg text isn't
 * written.
 */
class Dxxwjz2Writer : public dxxwjz1::v1::Dxxwjz1Writer
{
 public:
  using Dxxwjz2WriterPtr = std::shared_ptr<Dxxwjz2Writer>;

  virtual ~Dxxwjz2Writer() = default;
  explicit Dxxwjz2Writer(int level = DEFAULT_LEVEL);

  static Dxxwjz2WriterPtr create();

 protected:
  virtual std::unique_ptr<dxxwjz1::v1::Dxxwjz1JsonEmitter> create_emitter(
      std::ostream& out, const WeightsDocument& rest,
      const std::string& filename) const override;

  virtual bool check(const WeightsDocument& rest,
                     const std::string& filename) const override;

  /// @brief The file of the network alone (the network to train) keeps
  /// neither the header nor the layers.
  virtual bool writes_weights(const WeightsDocument& document) const override;
};

using Dxxwjz2WriterPtr = Dxxwjz2Writer::Dxxwjz2WriterPtr;

}  // namespace darknet_adaptor::loaders::dxxwjz2::v2

#endif  // DARKNET_CXX_PORT_PROJECT_DXXWJZ2_V2_WRITER_CLASS_H
