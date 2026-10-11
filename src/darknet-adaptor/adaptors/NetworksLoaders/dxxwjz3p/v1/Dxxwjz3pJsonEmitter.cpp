#include "src/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz3p/v1/Dxxwjz3pJsonEmitter.h"

#include <cstdint>
#include <filesystem>
#include <string>
#include <utility>

#include "src/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz3p/v1/Dxxwjz3pFormat.h"

namespace darknet_adaptor::loaders::dxxwjz3p::v1
{

Dxxwjz3pJsonEmitter::Dxxwjz3pJsonEmitter(std::ostream& out,
                                         const NetworkDescription& network,
                                         const TrainingRecord& training,
                                         const Yolo4Annotations& annotations,
                                         std::filesystem::path directory)
    : Dxxwjz3JsonEmitter{out, network, training},
      mannotations{network, annotations, std::move(directory)}
{
}

const char* Dxxwjz3pJsonEmitter::format_name() const
{
  return Dxxwjz3pFormat::NAME;
}

int64_t Dxxwjz3pJsonEmitter::format_version() const
{
  return Dxxwjz3pFormat::VERSION;
}

void Dxxwjz3pJsonEmitter::members()
{
  Dxxwjz3JsonEmitter::members();
  mannotations.write([this](const std::string& text) { write(text); });
}

}  // namespace darknet_adaptor::loaders::dxxwjz3p::v1
