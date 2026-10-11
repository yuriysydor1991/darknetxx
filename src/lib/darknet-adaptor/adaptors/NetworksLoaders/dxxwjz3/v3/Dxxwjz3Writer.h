#ifndef DARKNET_CXX_PORT_PROJECT_DXXWJZ3_V3_WRITER_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_DXXWJZ3_V3_WRITER_CLASS_H

#include <memory>
#include <ostream>
#include <string>
#include <vector>

#include "src/darknet.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz2/v2/Dxxwjz2Writer.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/weights-model/WeightsDocument.h"

namespace darknet_adaptor::loaders::dxxwjz3::v3
{

/**
 * @brief The dxxwjz3 file writer (see the Dxxwjz3Format): writes the file
 * the way the Dxxwjz2Writer does, the arrays in the layers of the JSON of the
 * network cfg, of the trailing bytes. The network the file can't keep (see
 * the keeps()) isn't written.
 */
class Dxxwjz3Writer : public dxxwjz2::v2::Dxxwjz2Writer
{
 public:
  using Dxxwjz3WriterPtr = std::shared_ptr<Dxxwjz3Writer>;
  using Dxxwjz2Writer::save;

  virtual ~Dxxwjz3Writer() = default;
  explicit Dxxwjz3Writer(int level = DEFAULT_LEVEL);

  static Dxxwjz3WriterPtr create();

  /// @brief The save() of the network the file keeps (see the keeps()).
  virtual bool save(network& net, const std::string& filename, int cutoff,
                    bool save_ema, const WeightsDocument& rest) override;

  /**
   * @brief Tells the dxxwjz3 file keeps the network: the cfg of the JSON (see
   * the Dxxwjz3CfgJson) and the layers in the sections of their indexes and
   * types, the arrays of a layer are the members of it's section. Logs the
   * network it can't keep.
   */
  static bool keeps(const NetworkDescription& description,
                    const std::vector<WeightsLayer>& layers,
                    const std::string& filename);

  /// @brief The keeps() of the layers of the network before the cutoff.
  static bool keeps(const NetworkDescription& description, const network& net,
                    int cutoff, const std::string& filename);

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
