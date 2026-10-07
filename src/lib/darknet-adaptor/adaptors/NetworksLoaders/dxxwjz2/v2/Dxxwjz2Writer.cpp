#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz2/v2/Dxxwjz2Writer.h"

#include <memory>
#include <ostream>
#include <string>

#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz2/v2/Dxxwjz2JsonEmitter.h"
#include "src/lib/darknet-adaptor/adaptors/orig-includes.h"

namespace darknet_adaptor::loaders::dxxwjz2::v2
{

Dxxwjz2Writer::Dxxwjz2Writer(const int level) : Dxxwjz1Writer{level} {}

Dxxwjz2Writer::Dxxwjz2WriterPtr Dxxwjz2Writer::create()
{
  return std::make_shared<Dxxwjz2Writer>();
}

std::unique_ptr<dxxwjz1::v1::Dxxwjz1JsonEmitter> Dxxwjz2Writer::create_emitter(
    std::ostream& out, const WeightsDocument& rest) const
{
  return std::make_unique<Dxxwjz2JsonEmitter>(out, rest.network, rest.training);
}

bool Dxxwjz2Writer::check(const WeightsDocument& rest,
                          const std::string& filename) const
{
  if (rest.network.empty()) {
    CLOGE(
        "The dxxwjz2 file %s keeps the network description, which has no cfg "
        "text",
        filename.c_str());
    return false;
  }

  return true;
}

bool Dxxwjz2Writer::writes_weights(const WeightsDocument& document) const
{
  return !document.network_alone();
}

}  // namespace darknet_adaptor::loaders::dxxwjz2::v2
