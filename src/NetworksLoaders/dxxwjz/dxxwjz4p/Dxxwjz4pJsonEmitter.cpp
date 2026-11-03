#include "src/NetworksLoaders/dxxwjz/dxxwjz4p/Dxxwjz4pJsonEmitter.h"

#include <cstdint>
#include <filesystem>
#include <string>
#include <utility>

#include "src/NetworksLoaders/dxxwjz/dxxwjz4/Dxxwjz4Description.h"
#include "src/NetworksLoaders/dxxwjz/dxxwjz4p/Dxxwjz4pFormat.h"

namespace darknet_adaptor::loaders::dxxwjz4p::v1
{

Dxxwjz4pJsonEmitter::Dxxwjz4pJsonEmitter(std::ostream& out,
                                         const NetworkDescription& network,
                                         const TrainingRecord& training,
                                         const Yolo4Annotations& annotations,
                                         std::filesystem::path directory,
                                         const std::string& description)
    : Dxxwjz3pJsonEmitter{out, network, training, annotations,
                          std::move(directory)},
      mdescription{description}
{
}

const char* Dxxwjz4pJsonEmitter::format_name() const
{
  return Dxxwjz4pFormat::NAME;
}

int64_t Dxxwjz4pJsonEmitter::format_version() const
{
  return Dxxwjz4pFormat::VERSION;
}

void Dxxwjz4pJsonEmitter::sections()
{
  write(",\n  " + dxxwjz4::v4::Dxxwjz4Description::member(mdescription));
  Dxxwjz3pJsonEmitter::sections();
}

}  // namespace darknet_adaptor::loaders::dxxwjz4p::v1
