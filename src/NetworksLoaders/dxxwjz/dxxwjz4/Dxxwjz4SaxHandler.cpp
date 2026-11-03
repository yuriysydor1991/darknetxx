#include "src/NetworksLoaders/dxxwjz/dxxwjz4/Dxxwjz4SaxHandler.h"

#include <cstdint>
#include <string>

#include "src/NetworksLoaders/dxxwjz/dxxwjz4/Dxxwjz4Format.h"

namespace darknet_adaptor::loaders::dxxwjz4::v4
{

Dxxwjz4SaxHandler::Dxxwjz4SaxHandler(WeightsDocument& document,
                                     WeightsReport& report, const bool npeek)
    : Dxxwjz3SaxHandler{document, report, npeek},
      mdescription{[this](const std::string& message) { return fail(message); }}
{
}

const char* Dxxwjz4SaxHandler::format_name() const
{
  return Dxxwjz4Format::NAME;
}

int64_t Dxxwjz4SaxHandler::format_version() const
{
  return Dxxwjz4Format::VERSION;
}

Dxxwjz4SaxHandler::Delegate* Dxxwjz4SaxHandler::extension(
    const std::string& name)
{
  return name == Dxxwjz4Format::DESCRIPTION_KEY
             ? mdescription.start()
             : Dxxwjz3SaxHandler::extension(name);
}

bool Dxxwjz4SaxHandler::extension_end(const std::string& name)
{
  return name == Dxxwjz4Format::DESCRIPTION_KEY
             ? mdescription.read(document().description)
             : Dxxwjz3SaxHandler::extension_end(name);
}

}  // namespace darknet_adaptor::loaders::dxxwjz4::v4
