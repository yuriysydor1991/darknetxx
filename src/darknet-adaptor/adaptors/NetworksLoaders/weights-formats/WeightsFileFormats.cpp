#include "src/darknet-adaptor/adaptors/NetworksLoaders/weights-formats/WeightsFileFormats.h"

#include <algorithm>
#include <filesystem>
#include <memory>
#include <string>
#include <vector>

#include "src/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz1/v1/Dxxwjz1FileFormat.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz2/v2/Dxxwjz2FileFormat.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz2p/v1/Dxxwjz2pFileFormat.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz3/v3/Dxxwjz3FileFormat.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz3p/v1/Dxxwjz3pFileFormat.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/original-weights/OrigWeightsFileFormat.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/yolo4-folder/Yolo4FolderFormat.h"

namespace darknet_adaptor::loaders
{

const std::vector<IWeightsFileFormatPtr>& WeightsFileFormats::all()
{
  static const std::vector<IWeightsFileFormatPtr> formats{
      std::make_shared<OrigWeightsFileFormat>(),
      std::make_shared<dxxwjz1::v1::Dxxwjz1FileFormat>(),
      std::make_shared<dxxwjz2::v2::Dxxwjz2FileFormat>(),
      std::make_shared<dxxwjz2p::v1::Dxxwjz2pFileFormat>(),
      std::make_shared<dxxwjz3::v3::Dxxwjz3FileFormat>(),
      std::make_shared<dxxwjz3p::v1::Dxxwjz3pFileFormat>()};

  return formats;
}

const std::vector<IWeightsFileFormatPtr>& WeightsFileFormats::convertible()
{
  static const std::vector<IWeightsFileFormatPtr> formats = []() {
    std::vector<IWeightsFileFormatPtr> every = all();

    every.push_back(std::make_shared<yolo4::Yolo4FolderFormat>());

    return every;
  }();

  return formats;
}

IWeightsFileFormatPtr WeightsFileFormats::named(
    const std::string& name, const std::vector<IWeightsFileFormatPtr>& formats)
{
  const auto found = std::find_if(
      formats.begin(), formats.end(),
      [&name](const auto& format) { return format->name() == name; });

  return found == formats.end() ? nullptr : *found;
}

IWeightsFileFormatPtr WeightsFileFormats::of_file(const std::string& path)
{
  const std::string extension =
      std::filesystem::path{path}.extension().string();

  const auto found = std::find_if(all().begin(), all().end(),
                                  [&extension](const auto& format) {
                                    return format->extension() == extension;
                                  });

  return found == all().end() ? all().front() : *found;
}

}  // namespace darknet_adaptor::loaders
