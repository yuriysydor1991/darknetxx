#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz2/v2/Dxxwjz2SaxHandler.h"

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz1/v1/Dxxwjz1Format.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz2/v2/Dxxwjz2Format.h"

namespace darknet_adaptor::loaders::dxxwjz2::v2
{

using dxxwjz1::v1::Dxxwjz1Format;

Dxxwjz2SaxHandler::Dxxwjz2SaxHandler(WeightsDocument& document,
                                     WeightsReport& report, const bool npeek)
    : Dxxwjz1SaxHandler{document, report},
      mbuilder{[this](const std::string& message) { return fail(message); }},
      mmetadata{report},
      mpeek{npeek}
{
  // the header of the file of no weights, which may have none
  document.header = WeightsHeader::current(0U);
}

bool Dxxwjz2SaxHandler::complete(const bool parsed) const
{
  return mstopped || Dxxwjz1SaxHandler::complete(parsed);
}

const char* Dxxwjz2SaxHandler::format_name() const
{
  return Dxxwjz2Format::NAME;
}

int64_t Dxxwjz2SaxHandler::format_version() const
{
  return Dxxwjz2Format::VERSION;
}

std::vector<std::string> Dxxwjz2SaxHandler::required_keys() const
{
  return {Dxxwjz1Format::FORMAT_KEY, Dxxwjz1Format::VERSION_KEY,
          Dxxwjz2Format::NETWORK_KEY};
}

Dxxwjz2SaxHandler::Delegate* Dxxwjz2SaxHandler::extension(
    const std::string& name)
{
  if (name == Dxxwjz1Format::TRAILING_KEY) {
    return trailing();
  }

  if (name != Dxxwjz2Format::NETWORK_KEY &&
      name != Dxxwjz2Format::TRAINING_KEY) {
    return nullptr;
  }

  mbuilder.reset(name);

  return &mbuilder;
}

bool Dxxwjz2SaxHandler::extension_end(const std::string& name)
{
  if (name == Dxxwjz1Format::TRAILING_KEY) {
    return true;
  }

  const bool read =
      name == Dxxwjz2Format::NETWORK_KEY
          ? metadata().network(mbuilder.tree(), document().network)
          : metadata().training(mbuilder.tree(), document().training);

  // the tree of the losses of the long trainings isn't kept
  mbuilder.reset(name);

  if (!read) {
    return fail(metadata().error());
  }

  mnetwork = mnetwork || name == Dxxwjz2Format::NETWORK_KEY;

  return true;
}

bool Dxxwjz2SaxHandler::appended()
{
  mstopped = mpeek && mnetwork;

  return !mstopped;
}

Dxxwjz2MetadataReader& Dxxwjz2SaxHandler::metadata() { return mmetadata; }

Dxxwjz2SaxHandler::Delegate* Dxxwjz2SaxHandler::trailing() { return nullptr; }

}  // namespace darknet_adaptor::loaders::dxxwjz2::v2
