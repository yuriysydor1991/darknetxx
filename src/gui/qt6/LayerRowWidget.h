#ifndef DARKNETXX_PROJECT_GUI_QT6_LAYERROWWIDGET_CLASS_H
#define DARKNETXX_PROJECT_GUI_QT6_LAYERROWWIDGET_CLASS_H

#include <QFrame>
#include <QGridLayout>
#include <QLabel>
#include <QToolButton>
#include <functional>
#include <memory>

#include "src/gui/net-editor/NetworkModel.h"
#include "src/gui/qt6/QtOptionsGrid.h"

namespace darknetxxQt6
{

/**
 * @brief The row of a layer: the handle to drag it by onto the place of
 * another layer, the index, the button expanding the option rows (made on
 * the first expansion), the type and the options of the layer and the button
 * removing the layer.
 */
class LayerRowWidget : public QFrame
{
 public:
  /// @brief The handlers of the row: the value of an option changed, the
  /// remove button clicked, the layer of an index dropped onto the one of
  /// another index.
  struct Handlers
  {
    std::function<void()> edited;
    std::function<void(LayerRowWidget&)> removed;
    std::function<void(int, int)> moved;
  };

  LayerRowWidget(const net_editor::NetworkModel& nmodel,
                 net_editor::NetworkSectionPtr nsection, Handlers nhandlers);

  /// @brief Shows the index, the options and the wrong ones of the moment.
  void refresh();

  /// @brief Shows or hides the option rows.
  void expand(bool expanded);

  const net_editor::NetworkSectionPtr& section() const;
  QToolButton& expand_button();
  QToolButton& remove_button();
  QLabel& index_label();
  QLabel& summary_label();
  QLabel& errors_label();

  /// @brief The rows of the options, nullptr before the first expansion.
  QtOptionsGrid* options();

 protected:
  virtual void dragEnterEvent(QDragEnterEvent* event) override;
  virtual void dropEvent(QDropEvent* event) override;

 private:
  const net_editor::NetworkModel& model;
  const net_editor::NetworkSectionPtr msection;
  const Handlers handlers;
  QLabel* mindex;
  QToolButton* mexpand;
  QLabel* msummary;
  QLabel* merrors;
  QToolButton* mremove;
  QWidget* mbody;
  QGridLayout* mgrid;
  std::unique_ptr<QtOptionsGrid> moptions;
};

}  // namespace darknetxxQt6

#endif  // DARKNETXX_PROJECT_GUI_QT6_LAYERROWWIDGET_CLASS_H
