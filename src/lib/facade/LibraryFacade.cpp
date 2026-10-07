#include "LibraryFacade.h"

#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/weights-formats/WeightsFileFormats.h"
#include "src/lib/darknet-adaptor/adaptors/TrainingChart/TrainingChart.h"
#include "src/lib/libmain/LibFactory.h"
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

LibraryFacade::FormatsDescriptions LibraryFacade::conversion_formats()
{
  using darknet_adaptor::loaders::WeightsFileFormats;

  FormatsDescriptions formats;

  for (const auto& format : WeightsFileFormats::all()) {
    formats.emplace_back(format->name(), format->description());
  }

  return formats;
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

void LibraryFacade::init_logger(const logger::ILoggerPtr& appLogger)
{
  LOG_INIT_REAL_LOGGER(appLogger);
}

}  // namespace darknetxx011
