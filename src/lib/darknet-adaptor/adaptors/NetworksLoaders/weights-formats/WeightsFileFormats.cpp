#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/weights-formats/WeightsFileFormats.h"

#include <algorithm>
#include <filesystem>
#include <memory>
#include <string>
#include <vector>

#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz1/v1/Dxxwjz1FileFormat.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz2/v2/Dxxwjz2FileFormat.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/original-weights/OrigWeightsFileFormat.h"

namespace darknet_adaptor::loaders
{

const std::vector<IWeightsFileFormatPtr>& WeightsFileFormats::all()
{
  static const std::vector<IWeightsFileFormatPtr> formats{
      std::make_shared<OrigWeightsFileFormat>(),
      std::make_shared<dxxwjz1::v1::Dxxwjz1FileFormat>(),
      std::make_shared<dxxwjz2::v2::Dxxwjz2FileFormat>()};

  return formats;
}

IWeightsFileFormatPtr WeightsFileFormats::named(const std::string& name)
{
  const auto found = std::find_if(
      all().begin(), all().end(),
      [&name](const auto& format) { return format->name() == name; });

  return found == all().end() ? nullptr : *found;
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
