#ifndef DARKNET_CXX_PORT_PROJECT_DXXWJZ3P_V1_JSON_EMITTER_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_DXXWJZ3P_V1_JSON_EMITTER_CLASS_H

#include <cstdint>
#include <string>
#include <vector>

#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz2p/v1/Dxxwjz2pJsonEmitter.h"

namespace darknet_adaptor::loaders::dxxwjz3p::v1
{

/**
 * @brief Writes the JSON text of the dxxwjz3p document (see the
 * Dxxwjz3pFormat): the dxxwjz2p one of the dxxwjz3 network cfg and trailing
 * bytes (see the Dxxwjz3JsonEmitter).
 */
class Dxxwjz3pJsonEmitter : public dxxwjz2p::v1::Dxxwjz2pJsonEmitter
{
 public:
  using Dxxwjz2pJsonEmitter::Dxxwjz2pJsonEmitter;

  virtual ~Dxxwjz3pJsonEmitter() = default;

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

}  // namespace darknet_adaptor::loaders::dxxwjz3p::v1

#endif  // DARKNET_CXX_PORT_PROJECT_DXXWJZ3P_V1_JSON_EMITTER_CLASS_H
