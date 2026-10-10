#include "src/lib/training/DxxwjzProject.h"

#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/weights-formats/WeightsFileFormats.h"

namespace darknetxx_impl
{

DxxwjzProject::DxxwjzProject(const std::string& npath)
    : ATrainingProject{absolute_of(npath).parent_path()}
{
  darknet_adaptor::loaders::WeightsDocument rest;
  bool weights = false;

  // the project of no weights is the network to train from the scratch
  if (darknet_adaptor::loaders::WeightsFileFormats::of_file(npath)
          ->create_reader()
          ->peek(npath, rest, weights)) {
    const std::string project = absolute_of(npath).string();

    train(project, weights ? project : std::string{});
  }
}

std::string DxxwjzProject::names() const { return {}; }

}  // namespace darknetxx_impl
