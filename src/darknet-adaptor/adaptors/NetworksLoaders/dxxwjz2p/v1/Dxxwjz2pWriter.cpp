#include "src/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz2p/v1/Dxxwjz2pWriter.h"

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <memory>
#include <ostream>
#include <string>

#include "src/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz2p/v1/Dxxwjz2pJsonEmitter.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz2p/v1/Dxxwjz2pParser.h"
#include "src/darknet-adaptor/adaptors/orig-includes.h"

namespace darknet_adaptor::loaders::dxxwjz2p::v1
{

Dxxwjz2pWriter::Dxxwjz2pWriter(const int level) : Dxxwjz2Writer{level} {}

Dxxwjz2pWriter::Dxxwjz2pWriterPtr Dxxwjz2pWriter::create()
{
  return std::make_shared<Dxxwjz2pWriter>();
}

std::unique_ptr<dxxwjz1::v1::Dxxwjz1JsonEmitter> Dxxwjz2pWriter::create_emitter(
    std::ostream& out, const WeightsDocument& rest,
    const std::string& filename) const
{
  return std::make_unique<Dxxwjz2pJsonEmitter>(
      out, rest.network, rest.training, rest.annotations,
      Dxxwjz2pParser::directory_of(filename));
}

bool Dxxwjz2pWriter::check(const WeightsDocument& rest,
                           const std::string& filename) const
{
  if (!Dxxwjz2Writer::check(rest, filename)) {
    return false;
  }

  const std::size_t classes = rest.network.classes().size();

  for (const Yolo4Image& image : rest.annotations.images) {
    const bool sized = image.width > 0 && image.height > 0;
    const auto wrong = [sized, classes](const Yolo4Box& box) {
      return !sized || box.id < 0 ||
             static_cast<std::size_t>(box.id) >= classes ||
             !std::isfinite(box.x + box.y + box.w + box.h);
    };

    if (!std::filesystem::path{image.path}.has_filename() ||
        std::any_of(image.boxes.begin(), image.boxes.end(), wrong)) {
      CLOGE(
          "The dxxwjz2p file %s can't keep the image %s: the file of the boxes "
          "of the network class names in the pixels of the image size",
          filename.c_str(), image.path.c_str());
      return false;
    }
  }

  return true;
}

}  // namespace darknet_adaptor::loaders::dxxwjz2p::v1
