#include "src/NetworksLoaders/dxxwjz/dxxwjz4/Dxxwjz4JsonEmitter.h"

#include <cstdint>
#include <ostream>
#include <string>

#include "src/NetworksLoaders/dxxwjz/dxxwjz4/Dxxwjz4Description.h"
#include "src/NetworksLoaders/dxxwjz/dxxwjz4/Dxxwjz4Format.h"

namespace darknet_adaptor::loaders::dxxwjz4::v4
{

Dxxwjz4JsonEmitter::Dxxwjz4JsonEmitter(std::ostream& out,
                                       const NetworkDescription& network,
                                       const TrainingRecord& training,
                                       const std::string& description)
    : Dxxwjz3JsonEmitter{out, network, training}, mdescription{description}
{
}

const char* Dxxwjz4JsonEmitter::format_name() const
{
  return Dxxwjz4Format::NAME;
}

int64_t Dxxwjz4JsonEmitter::format_version() const
{
  return Dxxwjz4Format::VERSION;
}

void Dxxwjz4JsonEmitter::sections()
{
  write(",\n  " + Dxxwjz4Description::member(mdescription));
  Dxxwjz3JsonEmitter::sections();
}

}  // namespace darknet_adaptor::loaders::dxxwjz4::v4
