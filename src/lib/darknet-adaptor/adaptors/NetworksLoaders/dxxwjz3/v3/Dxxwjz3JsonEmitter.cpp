#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz3/v3/Dxxwjz3JsonEmitter.h"

#include <cstdint>
#include <string>
#include <vector>

#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz3/v3/Dxxwjz3Bytes.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz3/v3/Dxxwjz3CfgJson.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz3/v3/Dxxwjz3Format.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz3/v3/Dxxwjz3Losses.h"

namespace darknet_adaptor::loaders::dxxwjz3::v3
{

const char* Dxxwjz3JsonEmitter::format_name() const
{
  return Dxxwjz3Format::NAME;
}

int64_t Dxxwjz3JsonEmitter::format_version() const
{
  return Dxxwjz3Format::VERSION;
}

std::string Dxxwjz3JsonEmitter::cfg(const NetworkDescription& network) const
{
  return Dxxwjz3CfgJson::members(network.cfg());
}

std::string Dxxwjz3JsonEmitter::trailing(
    const std::vector<uint8_t>& bytes) const
{
  return Dxxwjz3Bytes::json_of(bytes);
}

void Dxxwjz3JsonEmitter::losses(const TrainingRecord& training)
{
  columns(Dxxwjz3Losses::indexed(training.iterations(), training.losses()),
          Dxxwjz3Losses::indexed(training.iterations(), training.avg_losses()));
}

}  // namespace darknet_adaptor::loaders::dxxwjz3::v3
