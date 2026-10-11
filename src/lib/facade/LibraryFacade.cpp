#include "LibraryFacade.h"

#include <filesystem>
#include <memory>

#include "Yolo4Folder.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz2p/v1/Dxxwjz2pFormat.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz3p/v1/Dxxwjz3pFormat.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/weights-formats/NetworkSource.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/weights-formats/WeightsFileFormats.h"
#include "src/lib/darknet-adaptor/adaptors/TrainingChart/TrainingChart.h"
#include "src/lib/libmain/LibFactory.h"
#include "src/lib/training/DxxwjzProject.h"
#include "src/log/log.h"

namespace darknetxx011
{

LibraryContextPtr LibraryFacade::create_library_context()
{
  return darknetxx_impl::LibFactory::create_factory()->create_default_context();
}

ILibPtr LibraryFacade::create_library(LibraryContextPtr ctx)
{
  return darknetxx_impl::LibFactory::create_factory()->create_library(ctx);
}

ITrainerPtr LibraryFacade::create_trainer(LibraryContextPtr ctx)
{
  return darknetxx_impl::LibFactory::create_factory()->create_trainer(ctx);
}

ATrainingProjectPtr LibraryFacade::open_training_project(
    const std::string& path)
{
  using darknet_adaptor::loaders::dxxwjz2p::v1::Dxxwjz2pFormat;
  using darknet_adaptor::loaders::dxxwjz3p::v1::Dxxwjz3pFormat;

  const std::filesystem::path extension =
      std::filesystem::path{path}.extension();

  if (extension == Dxxwjz2pFormat::EXTENSION ||
      extension == Dxxwjz3pFormat::EXTENSION) {
    return std::make_shared<darknetxx_impl::DxxwjzProject>(path);
  }

  return std::make_shared<Yolo4Folder>(path);
}

IRecognizerPtr LibraryFacade::create_recognizer()
{
  return darknetxx_impl::LibFactory::create_factory()->create_recognizer();
}

LibraryFacade::FormatsDescriptions LibraryFacade::conversion_formats()
{
  using darknet_adaptor::loaders::WeightsFileFormats;

  FormatsDescriptions formats;

  for (const auto& format : WeightsFileFormats::convertible()) {
    formats.emplace_back(format->name(), format->description());
  }

  return formats;
}

std::vector<std::string> LibraryFacade::weights_extensions()
{
  using darknet_adaptor::loaders::WeightsFileFormats;

  std::vector<std::string> extensions;

  for (const auto& format : WeightsFileFormats::all()) {
    extensions.push_back(format->extension());
  }

  return extensions;
}

std::vector<unsigned char> LibraryFacade::training_chart(
    const std::string& weights)
{
  std::vector<unsigned char> png;

  if (!darknet_adaptor::adaptors::TrainingChart::render_file(weights, png)) {
    png.clear();
  }

  return png;
}

std::string LibraryFacade::network_cfg(const std::string& weights)
{
  using darknet_adaptor::loaders::NetworkSource;

  darknet_adaptor::loaders::NetworkDescription description;

  // the read of the other files takes their whole text for the cfg one
  return NetworkSource::describes(weights) &&
                 NetworkSource::read(weights, description)
             ? description.cfg()
             : std::string{};
}

void LibraryFacade::init_logger(const logger::ILoggerPtr& appLogger)
{
  LOG_INIT_REAL_LOGGER(appLogger);
}

}  // namespace darknetxx011
