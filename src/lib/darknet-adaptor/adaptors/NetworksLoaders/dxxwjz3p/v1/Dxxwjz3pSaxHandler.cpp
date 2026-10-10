#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz3p/v1/Dxxwjz3pSaxHandler.h"

#include <cstdint>
#include <filesystem>
#include <string>
#include <utility>

#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz3p/v1/Dxxwjz3pFormat.h"

namespace darknet_adaptor::loaders::dxxwjz3p::v1
{

Dxxwjz3pSaxHandler::Dxxwjz3pSaxHandler(WeightsDocument& document,
                                       WeightsReport& report,
                                       std::filesystem::path directory,
                                       const bool npeek)
    : Dxxwjz2pSaxHandler{document, report, std::move(directory), npeek},
      mdescription{report},
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

dxxwjz2::v2::Dxxwjz2MetadataReader& Dxxwjz3pSaxHandler::metadata()
{
  return mdescription;
}

Dxxwjz3pSaxHandler::Delegate* Dxxwjz3pSaxHandler::trailing() { return &mbytes; }

}  // namespace darknet_adaptor::loaders::dxxwjz3p::v1
