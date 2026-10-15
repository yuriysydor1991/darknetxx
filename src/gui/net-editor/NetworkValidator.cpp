#include "src/gui/net-editor/NetworkValidator.h"

#include "src/gui/net-editor/OptionPresenter.h"

namespace net_editor
{

std::vector<std::string> NetworkValidator::errors(const NetworkModel& model)
{
  std::vector<NetworkSectionPtr> sections{model.net()};
  std::vector<std::string> found;

  sections.insert(sections.end(), model.layers().begin(), model.layers().end());

  for (const auto& section : sections) {
    for (const auto& row : OptionPresenter::of(model, section)) {
      const std::string error = row->error();

      if (!error.empty()) {
        found.push_back(name_of(model, *section) + " " + row->key() + ": " +
                        error);
      }
    }
  }

  return found;
}

std::string NetworkValidator::name_of(const NetworkModel& model,
                                      const NetworkSection& section)
{
  const int index = model.index_of(section);

  return (index < 0 ? "" : "layer " + std::to_string(index) + " ") + "[" +
         section.type() + "]";
}

}  // namespace net_editor
