#include "src/gui/gtkmm4/net-editor-window/OptionsGrid.h"

namespace darknetxxGtkmm4::window::widgets
{

OptionsGrid::OptionsGrid(Gtk::Grid& grid, const net_editor::NetworkModel& model,
                         const net_editor::NetworkSectionPtr& section,
                         const std::function<void()>& edited)
{
  for (const auto& presenter :
       net_editor::OptionPresenter::of(model, section)) {
    mrows.push_back(std::make_unique<OptionRow>(
        grid, static_cast<int>(mrows.size()), presenter, edited));
  }
}

void OptionsGrid::refresh()
{
  for (const auto& row : mrows) {
    row->refresh();
  }
}

const std::vector<std::unique_ptr<OptionRow>>& OptionsGrid::rows() const
{
  return mrows;
}

}  // namespace darknetxxGtkmm4::window::widgets
