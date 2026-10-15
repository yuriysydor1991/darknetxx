#ifndef DARKNETXX_PROJECT_GUI_GTKMM4_OPTIONROW_CLASS_H
#define DARKNETXX_PROJECT_GUI_GTKMM4_OPTIONROW_CLASS_H

#include <gtkmm/dropdown.h>
#include <gtkmm/entry.h>
#include <gtkmm/grid.h>
#include <gtkmm/label.h>
#include <gtkmm/stringlist.h>
#include <sigc++/trackable.h>

#include <functional>
#include <string>
#include <vector>

#include "src/gui/net-editor/OptionPresenter.h"

namespace darknetxxGtkmm4::window::widgets
{

/**
 * @brief The row of an option in the grid of a section: the key, the entry
 * of the typed values (which takes the characters of the value alone) or
 * the drop down of the listed ones, and the description of the values the
 * option takes with the reason the value is wrong.
 */
class OptionRow : public sigc::trackable
{
 public:
  /// @param nedited Called after the value of the row changed.
  OptionRow(Gtk::Grid& grid, int row, net_editor::OptionPresenterPtr npresenter,
            std::function<void()> nedited);

  /// @brief Shows the limits of the moment and the reason the value is
  /// wrong.
  void refresh();

  const net_editor::OptionPresenter& presenter() const;
  Gtk::Entry* entry() const;
  Gtk::DropDown* dropdown() const;
  Gtk::Label& description() const;
  Gtk::Label& error() const;

 private:
  void on_insert(const Glib::ustring& text, int* position);
  void on_changed();
  void on_selected();

  const net_editor::OptionPresenterPtr mpresenter;
  const std::function<void()> edited;
  /// the listed values of the drop down, the empty one of the default first
  std::vector<std::string> mchoices;
  Gtk::Entry* mentry{nullptr};
  Gtk::DropDown* mdropdown{nullptr};
  Gtk::Label* mdescription{nullptr};
  Gtk::Label* merror{nullptr};
};

}  // namespace darknetxxGtkmm4::window::widgets

#endif  // DARKNETXX_PROJECT_GUI_GTKMM4_OPTIONROW_CLASS_H
