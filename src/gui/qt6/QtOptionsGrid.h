#ifndef DARKNETXX_PROJECT_GUI_QT6_QTOPTIONSGRID_CLASS_H
#define DARKNETXX_PROJECT_GUI_QT6_QTOPTIONSGRID_CLASS_H

#include <QGridLayout>
#include <functional>
#include <memory>
#include <vector>

#include "src/gui/net-editor/NetworkModel.h"
#include "src/gui/qt6/QtOptionRow.h"

namespace darknetxxQt6
{

/// @brief The rows of the options of a section in a grid (see the
/// OptionPresenter::of()).
class QtOptionsGrid
{
 public:
  /// @param edited Called after the value of a row changed.
  QtOptionsGrid(QGridLayout& grid, const net_editor::NetworkModel& model,
                const net_editor::NetworkSectionPtr& section,
                const std::function<void()>& edited);

  /// @brief Refreshes every row.
  void refresh();

  const std::vector<std::unique_ptr<QtOptionRow>>& rows() const;

 private:
  std::vector<std::unique_ptr<QtOptionRow>> mrows;
};

}  // namespace darknetxxQt6

#endif  // DARKNETXX_PROJECT_GUI_QT6_QTOPTIONSGRID_CLASS_H
