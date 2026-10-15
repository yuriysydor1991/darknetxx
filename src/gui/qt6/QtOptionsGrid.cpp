#include "src/gui/qt6/QtOptionsGrid.h"

namespace darknetxxQt6
{

QtOptionsGrid::QtOptionsGrid(QGridLayout& grid,
                             const net_editor::NetworkModel& model,
                             const net_editor::NetworkSectionPtr& section,
                             const std::function<void()>& edited)
{
  grid.setColumnStretch(2, 1);

  for (const auto& presenter :
       net_editor::OptionPresenter::of(model, section)) {
    mrows.push_back(std::make_unique<QtOptionRow>(
        grid, static_cast<int>(mrows.size()), presenter, edited));
  }
}

void QtOptionsGrid::refresh()
{
  for (const auto& row : mrows) {
    row->refresh();
  }
}

const std::vector<std::unique_ptr<QtOptionRow>>& QtOptionsGrid::rows() const
{
  return mrows;
}

}  // namespace darknetxxQt6
