#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz2p/v1/Dxxwjz2pSaxHandler.h"

#include <string>
#include <utility>
#include <vector>

#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz2/v2/Dxxwjz2Format.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz2p/v1/Dxxwjz2pFormat.h"

namespace darknet_adaptor::loaders::dxxwjz2p::v1
{

Dxxwjz2pSaxHandler::Dxxwjz2pSaxHandler(WeightsDocument& document,
                                       WeightsReport& report,
                                       std::filesystem::path directory,
                                       const bool npeek)
    : Dxxwjz2SaxHandler{document, report, npeek},
      mbuilder{[this](const std::string& message) { return fail(message); }},
      mreader{report, std::move(directory)}
{
}

const char* Dxxwjz2pSaxHandler::format_name() const
{
  return Dxxwjz2pFormat::NAME;
}

int64_t Dxxwjz2pSaxHandler::format_version() const
{
  return Dxxwjz2pFormat::VERSION;
}

std::vector<std::string> Dxxwjz2pSaxHandler::required_keys() const
{
  std::vector<std::string> keys = Dxxwjz2SaxHandler::required_keys();

  keys.emplace_back(Dxxwjz2pFormat::ANNOTATIONS_KEY);

  return keys;
}

Dxxwjz2pSaxHandler::Delegate* Dxxwjz2pSaxHandler::extension(
    const std::string& name)
{
  if (name != Dxxwjz2pFormat::ANNOTATIONS_KEY) {
    return Dxxwjz2SaxHandler::extension(name);
  }

  mbuilder.reset(name);
  return &mbuilder;
}

bool Dxxwjz2pSaxHandler::extension_end(const std::string& name)
{
  if (name != Dxxwjz2pFormat::ANNOTATIONS_KEY) {
    mnetwork = mnetwork || name == dxxwjz2::v2::Dxxwjz2Format::NETWORK_KEY;

    // the annotations read before the network of their classes
    return Dxxwjz2SaxHandler::extension_end(name) && annotate();
  }

  mannotated = true;
  mpending = true;
  return annotate();
}

bool Dxxwjz2pSaxHandler::appended()
{
  // the peek goes on to the annotations after the weights
  return !mannotated || Dxxwjz2SaxHandler::appended();
}

bool Dxxwjz2pSaxHandler::annotate()
{
  if (!mpending || !mnetwork) {
    return true;
  }

  mpending = false;

  const bool read = mreader.annotations(
      mbuilder.tree(), document().network.classes(), document().annotations);

  // the tree of the images of the big data sets isn't kept
  mbuilder.reset(Dxxwjz2pFormat::ANNOTATIONS_KEY);

  return read || fail(mreader.error());
}

}  // namespace darknet_adaptor::loaders::dxxwjz2p::v1
