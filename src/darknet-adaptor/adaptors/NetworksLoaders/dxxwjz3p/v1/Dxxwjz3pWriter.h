#ifndef DARKNET_CXX_PORT_PROJECT_DXXWJZ3P_V1_WRITER_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_DXXWJZ3P_V1_WRITER_CLASS_H

#include <memory>
#include <ostream>
#include <string>

#include "src/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz2p/v1/Dxxwjz2pWriter.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/weights-model/WeightsDocument.h"
#include "src/darknet.h"

namespace darknet_adaptor::loaders::dxxwjz3p::v1
{

/**
 * @brief The dxxwjz3p file writer (see the Dxxwjz3pFormat): writes the file
 * the way the Dxxwjz2pWriter does, of the network the dxxwjz3 files keep (see
 * the Dxxwjz3Writer).
 */
class Dxxwjz3pWriter : public dxxwjz2p::v1::Dxxwjz2pWriter
{
 public:
  using Dxxwjz3pWriterPtr = std::shared_ptr<Dxxwjz3pWriter>;
  using Dxxwjz2pWriter::save;

  virtual ~Dxxwjz3pWriter() = default;
  explicit Dxxwjz3pWriter(int level = DEFAULT_LEVEL);

  static Dxxwjz3pWriterPtr create();

  /// @brief The save() of the network the file keeps (see the
  /// Dxxwjz3Writer::keeps()).
  virtual bool save(network& net, const std::string& filename, int cutoff,
                    bool save_ema, const WeightsDocument& rest) override;

 protected:
  virtual std::unique_ptr<dxxwjz1::v1::Dxxwjz1JsonEmitter> create_emitter(
      std::ostream& out, const WeightsDocument& rest,
      const std::string& filename) const override;

  virtual bool check(const WeightsDocument& rest,
                     const std::string& filename) const override;
};

using Dxxwjz3pWriterPtr = Dxxwjz3pWriter::Dxxwjz3pWriterPtr;

}  // namespace darknet_adaptor::loaders::dxxwjz3p::v1

#endif  // DARKNET_CXX_PORT_PROJECT_DXXWJZ3P_V1_WRITER_CLASS_H
