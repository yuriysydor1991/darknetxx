#ifndef DARKNET_CXX_PORT_PROJECT_DXXWJZ3_V3_JSON_EMITTER_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_DXXWJZ3_V3_JSON_EMITTER_CLASS_H

#include <cstdint>
#include <string>
#include <vector>

#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz2/v2/Dxxwjz2JsonEmitter.h"

namespace darknet_adaptor::loaders::dxxwjz3::v3
{

/**
 * @brief Writes the JSON text of the dxxwjz3 document (see the
 * Dxxwjz3Format): the dxxwjz2 one of the network cfg of the net and the
 * layers members (see the Dxxwjz3CfgJson), of the array of the trailing
 * bytes.
 */
class Dxxwjz3JsonEmitter : public dxxwjz2::v2::Dxxwjz2JsonEmitter
{
 public:
  using Dxxwjz2JsonEmitter::Dxxwjz2JsonEmitter;

  virtual ~Dxxwjz3JsonEmitter() = default;

 protected:
  virtual const char* format_name() const override;
  virtual int64_t format_version() const override;
  virtual std::string cfg(const NetworkDescription& network) const override;
  virtual std::string trailing(
      const std::vector<uint8_t>& bytes) const override;
  /// @brief The losses of the iterations of their indexes, of no iteration
  /// column (see the Dxxwjz3Losses).
  virtual void losses(const TrainingRecord& training) override;
};

}  // namespace darknet_adaptor::loaders::dxxwjz3::v3

#endif  // DARKNET_CXX_PORT_PROJECT_DXXWJZ3_V3_JSON_EMITTER_CLASS_H
