#ifndef DARKNETXX_PROJECT_GUI_GTKMM4_NETWORKEVENTSHANDLER_CLASS_H
#define DARKNETXX_PROJECT_GUI_GTKMM4_NETWORKEVENTSHANDLER_CLASS_H

#include <memory>
#include <vector>

#include "src/gui/gtkmm4/IWindowEventHandler.h"
#include "src/gui/gtkmm4/net-editor-window/LayerRow.h"
#include "src/gui/gtkmm4/net-editor-window/OptionsGrid.h"

namespace darknetxxGtkmm4
{
class NetEditorWindowContext;
}  // namespace darknetxxGtkmm4

namespace darknetxxGtkmm4::window::event_handlers
{

/**
 * @brief The views of the network of the project: the rows of the [net]
 * options, the class names, the rows of the layers, which the + button adds
 * the layer of the chosen type to, the - button of a row removes and the
 * drag of the handle of a row moves. Every edit refreshes the limits and the
 * wrong values of every view, since they depend on the rest of the network.
 */
class NetworkEventsHandler : public IWindowEventHandler
{
 public:
  virtual ~NetworkEventsHandler() = default;
  explicit NetworkEventsHandler(std::shared_ptr<NetEditorWindowContext> nmwctx);

  virtual bool subscribe() override;

  /// @brief The rows of the layers in their order.
  const std::vector<widgets::LayerRow*>& rows() const;

  /// @brief The rows of the [net] options.
  widgets::OptionsGrid& net_rows();

  /// @brief Adds the layer of the type of the drop down at the bottom.
  virtual void add_layer();
  virtual void remove_layer(widgets::LayerRow& row);

  /// @brief Moves the layer to the index, the row of the layer dropped onto
  /// the one of the index.
  virtual void move_layer(int from, int to);

 protected:
  /// @brief Shows the network of the project anew.
  virtual void show_network();

  /// @brief Refreshes the views after an edit of the network.
  virtual void changed();

 private:
  widgets::LayerRow* append(const net_editor::NetworkSectionPtr& section);
  void on_classes_changed();
  void on_type_selected();

  std::shared_ptr<NetEditorWindowContext> mwctx;
  std::unique_ptr<widgets::OptionsGrid> mnet;
  std::vector<widgets::LayerRow*> mrows;
  /// the class names shown, not edited
  bool mshowing{false};
};

}  // namespace darknetxxGtkmm4::window::event_handlers

#endif  // DARKNETXX_PROJECT_GUI_GTKMM4_NETWORKEVENTSHANDLER_CLASS_H
