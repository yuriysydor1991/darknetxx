#include "src/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz2p/v1/Dxxwjz2pAnnotationsJson.h"

#include <filesystem>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include "src/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz1/v1/Dxxwjz1FloatCodec.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz1/v1/Dxxwjz1Format.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz1/v1/Dxxwjz1JsonString.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz2/v2/Dxxwjz2Format.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz2p/v1/Dxxwjz2pFormat.h"

namespace darknet_adaptor::loaders::dxxwjz2p::v1
{

using dxxwjz1::v1::Dxxwjz1FloatCodec;
using dxxwjz1::v1::Dxxwjz1JsonString;
using F = Dxxwjz2pFormat;

namespace
{

std::string pixels(const double value)
{
  return Dxxwjz1FloatCodec::encode(static_cast<float>(value));
}

/// @brief The JSON text of the member key: "\"name\": ".
std::string key(const char* const name)
{
  return Dxxwjz1JsonString::quote(name) + ": ";
}

}  // namespace

Dxxwjz2pAnnotationsJson::Dxxwjz2pAnnotationsJson(
    const NetworkDescription& network, const Yolo4Annotations& annotations,
    std::filesystem::path ndirectory)
    : mnetwork{network},
      mannotations{annotations},
      directory{std::move(ndirectory)}
{
}

void Dxxwjz2pAnnotationsJson::write(const Sink& sink) const
{
  std::map<std::string, std::vector<const Yolo4Image*>> groups;

  for (const Yolo4Image& found : mannotations.images) {
    groups[db_of(found.path)].push_back(&found);
  }

  sink(",\n  " + key(F::ANNOTATIONS_KEY) + "{" + key(F::ANNOTATOR_KEY) + "{" +
       key(dxxwjz1::v1::Dxxwjz1Format::VERSION_KEY) +
       Dxxwjz1JsonString::quote(F::ANNOTATOR_VERSION) + "}, " +
       key(F::ANNOTATIONS_KEY) + "[");

  const char* separator = "\n    {";

  for (const auto& [db, images] : groups) {
    sink(separator + key(F::DB_KEY) + "{" + key(F::PATH_KEY) +
         Dxxwjz1JsonString::quote(db) + "}, " + key(F::ANNOTATIONS_KEY) + "[");
    separator = ",\n    {";

    for (std::size_t index = 0U; index < images.size(); ++index) {
      image(*images[index], index == 0U, sink);
    }

    sink("\n    ]}");
  }

  sink(groups.empty() ? "]}" : "\n  ]}");
}

void Dxxwjz2pAnnotationsJson::image(const Yolo4Image& image, const bool first,
                                    const Sink& sink) const
{
  sink(std::string{first ? "\n      {" : ",\n      {"} + key(F::REL_PATH_KEY) +
       Dxxwjz1JsonString::quote(
           std::filesystem::path{image.path}.filename().string()) +
       ", " + key(F::WIDTH_KEY) + std::to_string(image.width) + ", " +
       key(F::HEIGHT_KEY) + std::to_string(image.height) +
       (image.validation ? ", " + key(F::VALIDATION_KEY) + "true" : "") + ", " +
       key(F::ANNOTATIONS_KEY) + "[");

  for (std::size_t index = 0U; index < image.boxes.size(); ++index) {
    const Yolo4Box& box = image.boxes[index];
    const double width = static_cast<double>(box.w) * image.width;
    const double height = static_cast<double>(box.h) * image.height;

    // the top left corner and the size of the box in the pixels
    sink(std::string{index == 0U ? "{" : ", {"} +
         key(dxxwjz2::v2::Dxxwjz2Format::NAME_KEY) +
         Dxxwjz1JsonString::quote(
             mnetwork.classes()[static_cast<std::size_t>(box.id)]) +
         ", " + key(F::RECT_KEY) + "{" + key(F::X_KEY) +
         pixels(static_cast<double>(box.x) * image.width - width / 2) + ", " +
         key(F::Y_KEY) +
         pixels(static_cast<double>(box.y) * image.height - height / 2) + ", " +
         key(F::WIDTH_KEY) + pixels(width) + ", " + key(F::HEIGHT_KEY) +
         pixels(height) + "}}");
  }

  sink("]}");
}

std::string Dxxwjz2pAnnotationsJson::db_of(const std::string& path) const
{
  const std::filesystem::path parent =
      std::filesystem::path{path}.parent_path();
  const std::filesystem::path relative = parent.lexically_relative(directory);

  return relative.empty() || *relative.begin() == ".."
             ? parent.generic_string()
             : relative.generic_string();
}

}  // namespace darknet_adaptor::loaders::dxxwjz2p::v1
