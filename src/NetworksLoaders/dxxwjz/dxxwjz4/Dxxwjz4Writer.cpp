#include "src/NetworksLoaders/dxxwjz/dxxwjz4/Dxxwjz4Writer.h"

#include <memory>
#include <ostream>
#include <string>

#include "src/NetworksLoaders/dxxwjz/dxxwjz4/Dxxwjz4JsonEmitter.h"

namespace darknet_adaptor::loaders::dxxwjz4::v4
{

Dxxwjz4Writer::Dxxwjz4Writer(const int level) : Dxxwjz3Writer{level} {}

Dxxwjz4Writer::Dxxwjz4WriterPtr Dxxwjz4Writer::create()
{
  return std::make_shared<Dxxwjz4Writer>();
}

std::unique_ptr<dxxwjz1::v1::Dxxwjz1JsonEmitter> Dxxwjz4Writer::create_emitter(
    std::ostream& out, const WeightsDocument& rest, const std::string&) const
{
  return std::make_unique<Dxxwjz4JsonEmitter>(out, rest.network, rest.training,
                                              rest.description);
}

}  // namespace darknet_adaptor::loaders::dxxwjz4::v4
