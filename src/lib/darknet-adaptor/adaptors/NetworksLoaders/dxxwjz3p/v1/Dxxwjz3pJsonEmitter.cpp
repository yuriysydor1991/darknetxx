#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz3p/v1/Dxxwjz3pJsonEmitter.h"

#include <cstdint>
#include <string>
#include <vector>

#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz3/v3/Dxxwjz3Bytes.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz3/v3/Dxxwjz3CfgJson.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz3/v3/Dxxwjz3Losses.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz3p/v1/Dxxwjz3pFormat.h"

namespace darknet_adaptor::loaders::dxxwjz3p::v1
{

const char* Dxxwjz3pJsonEmitter::format_name() const
{
  return Dxxwjz3pFormat::NAME;
}

int64_t Dxxwjz3pJsonEmitter::format_version() const
{
  return Dxxwjz3pFormat::VERSION;
}

std::string Dxxwjz3pJsonEmitter::cfg(const NetworkDescription& network) const
{
  return dxxwjz3::v3::Dxxwjz3CfgJson::members(network.cfg());
}

std::string Dxxwjz3pJsonEmitter::trailing(
    const std::vector<uint8_t>& bytes) const
{
  return dxxwjz3::v3::Dxxwjz3Bytes::json_of(bytes);
}

void Dxxwjz3pJsonEmitter::losses(const TrainingRecord& training)
{
  columns(dxxwjz3::v3::Dxxwjz3Losses::indexed(training.iterations(),
                                              training.losses()),
          dxxwjz3::v3::Dxxwjz3Losses::indexed(training.iterations(),
                                              training.avg_losses()));
}

}  // namespace darknet_adaptor::loaders::dxxwjz3p::v1
