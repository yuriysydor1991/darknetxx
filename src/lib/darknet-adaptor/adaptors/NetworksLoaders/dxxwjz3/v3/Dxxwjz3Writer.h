#ifndef DARKNET_CXX_PORT_PROJECT_DXXWJZ3_V3_WRITER_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_DXXWJZ3_V3_WRITER_CLASS_H

#include <memory>
#include <ostream>
#include <string>

#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz2/v2/Dxxwjz2Writer.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/weights-model/WeightsDocument.h"

namespace darknet_adaptor::loaders::dxxwjz3::v3
{

/**
 * @brief The dxxwjz3 file writer (see the Dxxwjz3Format): writes the file
 * the way the Dxxwjz2Writer does, of the JSON of the network cfg and of the
 * trailing bytes. The rest of the cfg text the JSON can't keep (of no [net]
 * section first) isn't written.
 */
class Dxxwjz3Writer : public dxxwjz2::v2::Dxxwjz2Writer
{
 public:
  using Dxxwjz3WriterPtr = std::shared_ptr<Dxxwjz3Writer>;

  virtual ~Dxxwjz3Writer() = default;
  explicit Dxxwjz3Writer(int level = DEFAULT_LEVEL);

  static Dxxwjz3WriterPtr create();

  /// @brief Tells the dxxwjz3 file keeps the cfg of the network (see the
  /// Dxxwjz3CfgJson), logs the one it can't keep.
  static bool keeps_cfg(const NetworkDescription& network,
                        const std::string& filename);

 protected:
  virtual std::unique_ptr<dxxwjz1::v1::Dxxwjz1JsonEmitter> create_emitter(
      std::ostream& out, const WeightsDocument& rest,
      const std::string& filename) const override;

  virtual bool check(const WeightsDocument& rest,
                     const std::string& filename) const override;
};

using Dxxwjz3WriterPtr = Dxxwjz3Writer::Dxxwjz3WriterPtr;

}  // namespace darknet_adaptor::loaders::dxxwjz3::v3

#endif  // DARKNET_CXX_PORT_PROJECT_DXXWJZ3_V3_WRITER_CLASS_H
