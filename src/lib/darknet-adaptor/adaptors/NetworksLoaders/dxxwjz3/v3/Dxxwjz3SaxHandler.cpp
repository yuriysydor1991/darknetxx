#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz3/v3/Dxxwjz3SaxHandler.h"

#include <cstdint>
#include <string>

#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz3/v3/Dxxwjz3Format.h"

namespace darknet_adaptor::loaders::dxxwjz3::v3
{

Dxxwjz3SaxHandler::Dxxwjz3SaxHandler(WeightsDocument& document,
                                     WeightsReport& report, const bool npeek)
    : Dxxwjz2SaxHandler{document, report, npeek},
      mdescription{report},
      mbytes{document.trailing,
             [this](const std::string& message) { return fail(message); }}
{
}

const char* Dxxwjz3SaxHandler::format_name() const
{
  return Dxxwjz3Format::NAME;
}

int64_t Dxxwjz3SaxHandler::format_version() const
{
  return Dxxwjz3Format::VERSION;
}

dxxwjz2::v2::Dxxwjz2MetadataReader& Dxxwjz3SaxHandler::metadata()
{
  return mdescription;
}

Dxxwjz3SaxHandler::Delegate* Dxxwjz3SaxHandler::trailing() { return &mbytes; }

}  // namespace darknet_adaptor::loaders::dxxwjz3::v3
