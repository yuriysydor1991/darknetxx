#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz3/v3/Dxxwjz3Writer.h"

#include <memory>
#include <ostream>
#include <string>

#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz3/v3/Dxxwjz3CfgJson.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz3/v3/Dxxwjz3JsonEmitter.h"
#include "src/lib/darknet-adaptor/adaptors/orig-includes.h"

namespace darknet_adaptor::loaders::dxxwjz3::v3
{

Dxxwjz3Writer::Dxxwjz3Writer(const int level) : Dxxwjz2Writer{level} {}

Dxxwjz3Writer::Dxxwjz3WriterPtr Dxxwjz3Writer::create()
{
  return std::make_shared<Dxxwjz3Writer>();
}

bool Dxxwjz3Writer::keeps_cfg(const NetworkDescription& network,
                              const std::string& filename)
{
  if (!Dxxwjz3CfgJson::members(network.cfg()).empty()) {
    return true;
  }

  CLOGE(
      "The %s file keeps the JSON of the network cfg of the [net] section "
      "first and of the [type] sections, which the cfg text isn't",
      filename.c_str());

  return false;
}

std::unique_ptr<dxxwjz1::v1::Dxxwjz1JsonEmitter> Dxxwjz3Writer::create_emitter(
    std::ostream& out, const WeightsDocument& rest, const std::string&) const
{
  return std::make_unique<Dxxwjz3JsonEmitter>(out, rest.network, rest.training);
}

bool Dxxwjz3Writer::check(const WeightsDocument& rest,
                          const std::string& filename) const
{
  return Dxxwjz2Writer::check(rest, filename) &&
         keeps_cfg(rest.network, filename);
}

}  // namespace darknet_adaptor::loaders::dxxwjz3::v3
