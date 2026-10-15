#ifndef DARKNETXX_PROJECT_GUI_GTKMM4_OPTIONSGRID_CLASS_H
#define DARKNETXX_PROJECT_GUI_GTKMM4_OPTIONSGRID_CLASS_H

#include <gtkmm/grid.h>

#include <functional>
#include <memory>
#include <vector>

#include "src/gui/gtkmm4/net-editor-window/OptionRow.h"
#include "src/gui/net-editor/NetworkModel.h"

namespace darknetxxGtkmm4::window::widgets
{

/// @brief The rows of the options of a section in a grid (see the
/// OptionPresenter::of()).
class OptionsGrid
{
 public:
  /// @param edited Called after the value of a row changed.
  OptionsGrid(Gtk::Grid& grid, const net_editor::NetworkModel& model,
              const net_editor::NetworkSectionPtr& section,
              const std::function<void()>& edited);

  /// @brief Refreshes every row.
  void refresh();

  const std::vector<std::unique_ptr<OptionRow>>& rows() const;

 private:
  std::vector<std::unique_ptr<OptionRow>> mrows;
};

}  // namespace darknetxxGtkmm4::window::widgets

#endif  // DARKNETXX_PROJECT_GUI_GTKMM4_OPTIONSGRID_CLASS_H
