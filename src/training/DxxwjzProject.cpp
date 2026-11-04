#include "src/training/DxxwjzProject.h"

#include "src/NetworksLoaders/DataDriversAliases.h"

namespace darknetxx_impl
{

using dxxdd::NetworkSource;

DxxwjzProject::DxxwjzProject(const std::string& npath)
    : ATrainingProject{absolute_of(npath).parent_path()}
{
  dxxdd::WeightsDocument rest;
  bool weights = false;

  // the project of no weights is the network to train from the scratch
  if (dxxdd::WeightsFileFormats::of_file(npath)->create_reader()->peek(
          npath, rest, weights)) {
    const std::string project = absolute_of(npath).string();
    // the dxxwjz4 file of the latest weights of the dxxwjz4p project
    const std::string resumed = NetworkSource::resumed_of(project);

    train(project,
          resumed != project ? resumed : (weights ? project : std::string{}));
  }
}

std::string DxxwjzProject::names() const { return {}; }

}  // namespace darknetxx_impl
