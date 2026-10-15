#ifndef DARKNETXX_PROJECT_GUI_QT6_QTOPTIONROW_CLASS_H
#define DARKNETXX_PROJECT_GUI_QT6_QTOPTIONROW_CLASS_H

#include <QComboBox>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <functional>
#include <string>
#include <vector>

#include "src/gui/net-editor/OptionPresenter.h"

namespace darknetxxQt6
{

/**
 * @brief The row of an option in the grid of a section: the key, the line
 * edit of the typed values (which takes the characters of the value alone)
 * or the combo box of the listed ones, and the description of the values the
 * option takes with the reason the value is wrong.
 */
class QtOptionRow
{
 public:
  /// @param edited Called after the value of the row changed.
  QtOptionRow(QGridLayout& grid, int row,
              net_editor::OptionPresenterPtr npresenter,
              std::function<void()> edited);

  /// @brief Keeps the widgets of the row, which outlive it, from telling the
  /// row of their changes.
  ~QtOptionRow();

  /// @brief Shows the limits of the moment and the reason the value is
  /// wrong.
  void refresh();

  const net_editor::OptionPresenter& presenter() const;
  QLineEdit* line() const;
  QComboBox* combo() const;
  QLabel& description() const;
  QLabel& error() const;

 private:
  const net_editor::OptionPresenterPtr mpresenter;
  /// the listed values of the combo box, the empty one of the default first
  std::vector<std::string> mchoices;
  QLineEdit* mline{nullptr};
  QComboBox* mcombo{nullptr};
  QLabel* mdescription{nullptr};
  QLabel* merror{nullptr};
};

}  // namespace darknetxxQt6

#endif  // DARKNETXX_PROJECT_GUI_QT6_QTOPTIONROW_CLASS_H
