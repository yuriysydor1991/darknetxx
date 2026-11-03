#include "src/NetworksLoaders/dxxwjz/dxxwjz4p/Dxxwjz4pSaxHandler.h"

#include <cstdint>
#include <filesystem>
#include <string>
#include <utility>

#include "src/NetworksLoaders/dxxwjz/dxxwjz4/Dxxwjz4Format.h"
#include "src/NetworksLoaders/dxxwjz/dxxwjz4p/Dxxwjz4pFormat.h"

namespace darknet_adaptor::loaders::dxxwjz4p::v1
{

using dxxwjz4::v4::Dxxwjz4Format;

Dxxwjz4pSaxHandler::Dxxwjz4pSaxHandler(WeightsDocument& document,
                                       WeightsReport& report,
                                       std::filesystem::path directory,
                                       const bool npeek)
    : Dxxwjz3pSaxHandler{document, report, std::move(directory), npeek},
      mdescription{[this](const std::string& message) { return fail(message); }}
{
}

const char* Dxxwjz4pSaxHandler::format_name() const
{
  return Dxxwjz4pFormat::NAME;
}

int64_t Dxxwjz4pSaxHandler::format_version() const
{
  return Dxxwjz4pFormat::VERSION;
}

Dxxwjz4pSaxHandler::Delegate* Dxxwjz4pSaxHandler::extension(
    const std::string& name)
{
  return name == Dxxwjz4Format::DESCRIPTION_KEY
             ? mdescription.start()
             : Dxxwjz3pSaxHandler::extension(name);
}

bool Dxxwjz4pSaxHandler::extension_end(const std::string& name)
{
  return name == Dxxwjz4Format::DESCRIPTION_KEY
             ? mdescription.read(document().description)
             : Dxxwjz3pSaxHandler::extension_end(name);
}

}  // namespace darknet_adaptor::loaders::dxxwjz4p::v1
