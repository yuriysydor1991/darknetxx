#include "src/gui/net-editor/NetworkProject.h"

#include <filesystem>
#include <vector>

#include "LibraryFacade.h"
#include "src/gui/net-editor/NetworkValidator.h"

namespace net_editor
{

NetworkProject::NetworkProject() { create(); }

void NetworkProject::create()
{
  model = NetworkModel{};
  model.read(
      "[net]\nbatch=64\nsubdivisions=16\nwidth=416\nheight=416\nchannels=3\n"
      "momentum=0.9\ndecay=0.0005\nlearning_rate=0.001\nburn_in=1000\n"
      "max_batches=6000\npolicy=steps\nsteps=4800,5400\nscales=0.1,0.1\n");
  mpath.clear();
  msaved = snapshot();
}

bool NetworkProject::open(const std::string& path, std::string& message)
{
  std::string cfg;
  NetworkModel opened;

  if (!darknetxx011::LibraryFacade::network_description(path, cfg,
                                                        opened.classes) ||
      !opened.read(cfg)) {
    message = "The " + path + " file keeps no network";
    return false;
  }

  model = std::move(opened);
  mpath = path;
  msaved = snapshot();

  return true;
}

bool NetworkProject::save(const std::string& path, std::string& message)
{
  static constexpr const std::size_t TOLD = 10U;
  const std::vector<std::string> errors = NetworkValidator::errors(model);

  if (!errors.empty()) {
    message =
        "The network has " + std::to_string(errors.size()) + " wrong options:";

    for (std::size_t error = 0U; error < errors.size() && error < TOLD;
         ++error) {
      message += "\n" + errors[error];
    }

    return false;
  }

  if (!darknetxx011::LibraryFacade::save_network(mpath, path, model.cfg(),
                                                 model.classes, message)) {
    return false;
  }

  mpath = path;
  msaved = snapshot();

  return true;
}

std::string NetworkProject::project_of(const std::string& chosen)
{
  std::filesystem::path path{chosen};

  if (!path.has_extension()) {
    path += EXTENSION;
  }

  return path.string();
}

const std::string& NetworkProject::path() const { return mpath; }

bool NetworkProject::modified() const { return snapshot() != msaved; }

std::string NetworkProject::title() const
{
  return (mpath.empty() ? std::string{"New network"}
                        : std::filesystem::path{mpath}.filename().string()) +
         (modified() ? " *" : "");
}

std::string NetworkProject::snapshot() const
{
  std::string text = model.cfg();

  for (const auto& name : model.classes) {
    text += "\n" + name;
  }

  return text;
}

}  // namespace net_editor
