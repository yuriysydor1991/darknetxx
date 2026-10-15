#include "LibraryFacade.h"

#include <filesystem>
#include <memory>

#include "Yolo4Folder.h"
#include "src/NetworksLoaders/dxxwjz/dxxwjz2p/Dxxwjz2pFormat.h"
#include "src/NetworksLoaders/dxxwjz/dxxwjz3p/Dxxwjz3pFormat.h"
#include "src/NetworksLoaders/weights-converter/NetworkRewriter.h"
#include "src/NetworksLoaders/weights-formats/NetworkSource.h"
#include "src/NetworksLoaders/weights-formats/WeightsFileFormats.h"
#include "src/darknet-adaptor/adaptors/TrainingChart/TrainingChart.h"
#include "src/lib/libmain/LibFactory.h"
#include "src/log/log.h"
#include "src/training/DxxwjzProject.h"

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
  std::string cfg;
  std::vector<std::string> classes;

  return network_description(weights, cfg, classes) ? cfg : std::string{};
}

bool LibraryFacade::network_description(const std::string& weights,
                                        std::string& cfg,
                                        std::vector<std::string>& classes)
{
  using darknet_adaptor::loaders::NetworkSource;

  darknet_adaptor::loaders::NetworkDescription description;

  // the read of the other files takes their whole text for the cfg one
  if (!NetworkSource::describes(weights) ||
      !NetworkSource::read(weights, description)) {
    return false;
  }

  cfg = description.cfg();
  classes = description.classes();

  return true;
}

bool LibraryFacade::save_network(const std::string& source,
                                 const std::string& destination,
                                 const std::string& cfg,
                                 const std::vector<std::string>& classes,
                                 std::string& message)
{
  return darknet_adaptor::loaders::NetworkRewriter::rewrite(
      source, destination,
      darknet_adaptor::loaders::NetworkDescription{cfg, classes}, message);
}

void LibraryFacade::init_logger(const logger::ILoggerPtr& appLogger)
{
  LOG_INIT_REAL_LOGGER(appLogger);
}

}  // namespace darknetxx011
