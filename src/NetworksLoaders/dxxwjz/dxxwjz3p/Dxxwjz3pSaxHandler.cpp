#include "src/NetworksLoaders/dxxwjz/dxxwjz3p/Dxxwjz3pSaxHandler.h"

#include <cstdint>
#include <filesystem>
#include <string>
#include <utility>

#include "src/NetworksLoaders/dxxwjz/dxxwjz2/Dxxwjz2Format.h"
#include "src/NetworksLoaders/dxxwjz/dxxwjz3p/Dxxwjz3pFormat.h"

namespace darknet_adaptor::loaders::dxxwjz3p::v1
{

Dxxwjz3pSaxHandler::Dxxwjz3pSaxHandler(WeightsDocument& document,
                                       WeightsReport& report,
                                       std::filesystem::path directory,
                                       const bool npeek)
    : Dxxwjz2pSaxHandler{document, report, std::move(directory), npeek},
      mdescription{report},
      mnetwork{[this](const std::string& message) { return fail(message); },
               mdescription, document.layers},
      mbytes{document.trailing,
             [this](const std::string& message) { return fail(message); }}
{
}

const char* Dxxwjz3pSaxHandler::format_name() const
{
  return Dxxwjz3pFormat::NAME;
}

int64_t Dxxwjz3pSaxHandler::format_version() const
{
  return Dxxwjz3pFormat::VERSION;
}

bool Dxxwjz3pSaxHandler::layers_member() const { return false; }

dxxwjz2::v2::Dxxwjz2MetadataReader& Dxxwjz3pSaxHandler::metadata()
{
  return mdescription;
}

dxxwjz2::v2::Dxxwjz2SubtreeBuilder& Dxxwjz3pSaxHandler::builder(
    const std::string& name)
{
  return name == dxxwjz2::v2::Dxxwjz2Format::NETWORK_KEY
             ? mnetwork
             : Dxxwjz2pSaxHandler::builder(name);
}

Dxxwjz3pSaxHandler::Delegate* Dxxwjz3pSaxHandler::trailing() { return &mbytes; }

}  // namespace darknet_adaptor::loaders::dxxwjz3p::v1
