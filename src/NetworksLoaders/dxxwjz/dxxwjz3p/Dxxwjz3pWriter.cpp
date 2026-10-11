#include "src/NetworksLoaders/dxxwjz/dxxwjz3p/Dxxwjz3pWriter.h"

#include <memory>
#include <ostream>
#include <string>

#include "src/NetworksLoaders/dxxwjz/dxxwjz2p/Dxxwjz2pParser.h"
#include "src/NetworksLoaders/dxxwjz/dxxwjz3/Dxxwjz3Writer.h"
#include "src/NetworksLoaders/dxxwjz/dxxwjz3p/Dxxwjz3pJsonEmitter.h"

namespace darknet_adaptor::loaders::dxxwjz3p::v1
{

Dxxwjz3pWriter::Dxxwjz3pWriter(const int level) : Dxxwjz2pWriter{level} {}

Dxxwjz3pWriter::Dxxwjz3pWriterPtr Dxxwjz3pWriter::create()
{
  return std::make_shared<Dxxwjz3pWriter>();
}

bool Dxxwjz3pWriter::save(network& net, const std::string& filename,
                          const int cutoff, const bool save_ema,
                          const WeightsDocument& rest)
{
  return dxxwjz3::v3::Dxxwjz3Writer::keeps(rest.network, net, cutoff,
                                           filename) &&
         Dxxwjz2pWriter::save(net, filename, cutoff, save_ema, rest);
}

std::unique_ptr<dxxwjz1::v1::Dxxwjz1JsonEmitter> Dxxwjz3pWriter::create_emitter(
    std::ostream& out, const WeightsDocument& rest,
    const std::string& filename) const
{
  return std::make_unique<Dxxwjz3pJsonEmitter>(
      out, rest.network, rest.training, rest.annotations,
      dxxwjz2p::v1::Dxxwjz2pParser::directory_of(filename));
}

bool Dxxwjz3pWriter::check(const WeightsDocument& rest,
                           const std::string& filename) const
{
  return Dxxwjz2pWriter::check(rest, filename) &&
         dxxwjz3::v3::Dxxwjz3Writer::keeps(rest.network, rest.layers, filename);
}

}  // namespace darknet_adaptor::loaders::dxxwjz3p::v1
