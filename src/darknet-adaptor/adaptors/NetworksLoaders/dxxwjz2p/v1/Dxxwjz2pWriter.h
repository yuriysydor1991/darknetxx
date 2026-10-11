#ifndef DARKNET_CXX_PORT_PROJECT_DXXWJZ2P_V1_WRITER_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_DXXWJZ2P_V1_WRITER_CLASS_H

#include <memory>
#include <ostream>
#include <string>

#include "src/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz2/v2/Dxxwjz2Writer.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/weights-model/WeightsDocument.h"

namespace darknet_adaptor::loaders::dxxwjz2p::v1
{

/**
 * @brief The dxxwjz2p file writer (see the Dxxwjz2pFormat): writes the file
 * the way the Dxxwjz2Writer does with the annotations of the rest of the
 * save() and of the document. The boxes are written in the pixels of the
 * class names of the network, so the rest of the boxes of the images of no
 * size or of the classes of no name isn't written.
 */
class Dxxwjz2pWriter : public dxxwjz2::v2::Dxxwjz2Writer
{
 public:
  using Dxxwjz2pWriterPtr = std::shared_ptr<Dxxwjz2pWriter>;

  virtual ~Dxxwjz2pWriter() = default;
  explicit Dxxwjz2pWriter(int level = DEFAULT_LEVEL);

  static Dxxwjz2pWriterPtr create();

 protected:
  virtual std::unique_ptr<dxxwjz1::v1::Dxxwjz1JsonEmitter> create_emitter(
      std::ostream& out, const WeightsDocument& rest,
      const std::string& filename) const override;

  virtual bool check(const WeightsDocument& rest,
                     const std::string& filename) const override;
};

using Dxxwjz2pWriterPtr = Dxxwjz2pWriter::Dxxwjz2pWriterPtr;

}  // namespace darknet_adaptor::loaders::dxxwjz2p::v1

#endif  // DARKNET_CXX_PORT_PROJECT_DXXWJZ2P_V1_WRITER_CLASS_H
