#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz2p/v1/Dxxwjz2pJsonEmitter.h"

#include <filesystem>
#include <string>
#include <utility>

#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz2p/v1/Dxxwjz2pFormat.h"

namespace darknet_adaptor::loaders::dxxwjz2p::v1
{

Dxxwjz2pJsonEmitter::Dxxwjz2pJsonEmitter(std::ostream& out,
                                         const NetworkDescription& network,
                                         const TrainingRecord& training,
                                         const Yolo4Annotations& annotations,
                                         std::filesystem::path directory)
    : Dxxwjz2JsonEmitter{out, network, training},
      mannotations{network, annotations, std::move(directory)}
{
}

const char* Dxxwjz2pJsonEmitter::format_name() const
{
  return Dxxwjz2pFormat::NAME;
}

int64_t Dxxwjz2pJsonEmitter::format_version() const
{
  return Dxxwjz2pFormat::VERSION;
}

void Dxxwjz2pJsonEmitter::sections()
{
  Dxxwjz2JsonEmitter::sections();
  mannotations.write([this](const std::string& text) { write(text); });
}

}  // namespace darknet_adaptor::loaders::dxxwjz2p::v1
