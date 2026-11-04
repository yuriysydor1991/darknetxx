#include "LibraryFacade.h"

#include <filesystem>
#include <memory>

#include "Yolo4Folder.h"
#include "src/NetworksLoaders/DataDriversAliases.h"
#include "src/NetworksLoaders/network-rewriter/NetworkRewriter.h"
#include "src/darknet-adaptor/adaptors/TrainingChart/TrainingChart.h"
#include "src/lib/libmain/LibFactory.h"
#include "src/log/log.h"
#include "src/training/DxxwjzProject.h"

namespace darknetxx012
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
  // the dxxwjz2p, the dxxwjz3p and the dxxwjz4p projects are the formats of
  // their file name extensions which keep the annotations, the rest is the
  // YOLO4 folder
  if (dxxdd::WeightsFileFormats::of_file(path)->keeps_annotations()) {
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
  using dxxdd::WeightsFileFormats;

  FormatsDescriptions formats;

  for (const auto& format : WeightsFileFormats::convertible()) {
    formats.emplace_back(format->name(), format->description());
  }

  return formats;
}

std::vector<std::string> LibraryFacade::weights_extensions()
{
  using dxxdd::WeightsFileFormats;

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
  using dxxdd::NetworkSource;

  dxxdd::NetworkDescription description;

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
      source, destination, dxxdd::NetworkDescription{cfg, classes}, message);
}

void LibraryFacade::init_logger(const logger::ILoggerPtr& appLogger)
{
  LOG_INIT_REAL_LOGGER(appLogger);

  // The darknetxx-datadrivers library carries an own copy of the logging
  // subsystem too, so the very same logger is handed over to it as well, and
  // the messages of the network files formats land in the log of the
  // application. The library calls the log() methods of the adopted logger
  // alone, which both of the logger::ILogger definitions (the one of this
  // project and the one of the cpp-app-template the library is made of) keep
  // in the very same slots of the virtual table.
  dxxdd::DxxDataDriversFacade::init_logger(appLogger);
}

}  // namespace darknetxx012
