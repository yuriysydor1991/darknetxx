#include "src/NetworksLoaders/dxxwjz/dxxwjz3/Dxxwjz3Writer.h"

#include <cstddef>
#include <memory>
#include <ostream>
#include <string>
#include <vector>

#include "src/NetworksLoaders/dxxwjz/dxxwjz3/Dxxwjz3CfgJson.h"
#include "src/NetworksLoaders/dxxwjz/dxxwjz3/Dxxwjz3JsonEmitter.h"
#include "src/NetworksLoaders/weights-model/WeightsSchema.h"
#include "src/darknet-adaptor/adaptors/orig-includes.h"

namespace darknet_adaptor::loaders::dxxwjz3::v3
{

Dxxwjz3Writer::Dxxwjz3Writer(const int level) : Dxxwjz2Writer{level} {}

Dxxwjz3Writer::Dxxwjz3WriterPtr Dxxwjz3Writer::create()
{
  return std::make_shared<Dxxwjz3Writer>();
}

bool Dxxwjz3Writer::save(network& net, const std::string& filename,
                         const int cutoff, const bool save_ema,
                         const WeightsDocument& rest)
{
  return keeps(rest.network, net, cutoff, filename) &&
         Dxxwjz2Writer::save(net, filename, cutoff, save_ema, rest);
}

bool Dxxwjz3Writer::keeps(const NetworkDescription& description,
                          const std::vector<WeightsLayer>& layers,
                          const std::string& filename)
{
  const Dxxwjz3CfgJson cfg{description.cfg()};

  if (!cfg.valid()) {
    CLOGE(
        "The %s file keeps the JSON of the network cfg of the [net] section "
        "first and of the [type] sections, which the cfg text isn't",
        filename.c_str());
    return false;
  }

  for (const WeightsLayer& l : layers) {
    const auto at = static_cast<std::size_t>(l.index);

    if (l.index < 0 || at >= cfg.types().size() || cfg.types()[at] != l.type) {
      CLOGE(
          "The %s file keeps the layers in the sections of the network cfg, "
          "the layer %d (%s) isn't the one of the cfg",
          filename.c_str(), l.index, WeightsSchema::name(l.type).c_str());
      return false;
    }
  }

  return true;
}

bool Dxxwjz3Writer::keeps(const NetworkDescription& description,
                          const network& net, const int cutoff,
                          const std::string& filename)
{
  std::vector<WeightsLayer> layers;

  for (int i = 0; i < net.n && i < cutoff; ++i) {
    layers.push_back({i, net.layers[i].type, {}});
  }

  return keeps(description, layers, filename);
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
         keeps(rest.network, rest.layers, filename);
}

}  // namespace darknet_adaptor::loaders::dxxwjz3::v3
