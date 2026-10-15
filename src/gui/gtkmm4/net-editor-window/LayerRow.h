#ifndef DARKNETXX_PROJECT_GUI_GTKMM4_LAYERROW_CLASS_H
#define DARKNETXX_PROJECT_GUI_GTKMM4_LAYERROW_CLASS_H

#include <gtkmm/box.h>
#include <gtkmm/button.h>
#include <gtkmm/expander.h>
#include <gtkmm/frame.h>
#include <gtkmm/grid.h>
#include <gtkmm/image.h>
#include <gtkmm/label.h>

#include <functional>
#include <memory>

#include "src/gui/gtkmm4/net-editor-window/OptionsGrid.h"
#include "src/gui/net-editor/NetworkModel.h"

namespace darknetxxGtkmm4::window::widgets
{

/**
 * @brief The row of a layer: the handle to drag it by onto the place of
 * another layer, the index, the type and the options of the layer in the
 * header of the expander, the button removing the layer and the option rows
 * below them the expander shows (made on the first expansion).
 */
class LayerRow : public Gtk::Frame
{
 public:
  /// @brief The handlers of the row: the value of an option changed, the
  /// remove button clicked, the layer of an index dropped onto the one of
  /// another index.
  struct Handlers
  {
    std::function<void()> edited;
    std::function<void(LayerRow&)> removed;
    std::function<void(int, int)> moved;
  };

  LayerRow(const net_editor::NetworkModel& nmodel,
           net_editor::NetworkSectionPtr nsection, Handlers nhandlers);

  /// @brief Shows the index, the options and the wrong ones of the moment.
  void refresh();

  const net_editor::NetworkSectionPtr& section() const;
  Gtk::Expander& expander();
  Gtk::Button& remove_button();
  Gtk::Label& index_label();
  Gtk::Label& summary_label();
  Gtk::Label& errors_label();

  /// @brief The rows of the options, nullptr before the first expansion.
  OptionsGrid* options();

 private:
  void on_expanded();

  const net_editor::NetworkModel& model;
  const net_editor::NetworkSectionPtr msection;
  const Handlers handlers;
  Gtk::Box mcolumn;
  Gtk::Box mrow;
  Gtk::Image mhandle;
  Gtk::Label mindex;
  Gtk::Expander mexpander;
  Gtk::Box mheader;
  Gtk::Label mtype;
  Gtk::Label msummary;
  Gtk::Label merrors;
  Gtk::Button mremove;
  Gtk::Box mbody;
  Gtk::Grid mgrid;
  std::unique_ptr<OptionsGrid> moptions;
};

}  // namespace darknetxxGtkmm4::window::widgets

#endif  // DARKNETXX_PROJECT_GUI_GTKMM4_LAYERROW_CLASS_H
