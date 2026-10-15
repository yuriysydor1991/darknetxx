#ifndef DARKNETXX_PROJECT_GUI_QT6_DRAGHANDLE_CLASS_H
#define DARKNETXX_PROJECT_GUI_QT6_DRAGHANDLE_CLASS_H

#include <QLabel>
#include <QPoint>
#include <functional>

namespace darknetxxQt6
{

/**
 * @brief The handle of a layer row the user drags the layer by: the drag of
 * the MIME data of the index of the layer.
 */
class DragHandle : public QLabel
{
 public:
  /// @brief The MIME type of the dragged layer index.
  inline static const QString MIME{"application/x-darknetxx-layer"};

  /// @param nindex The index of the layer of the moment.
  /// @param nrow The widget the drag shows.
  DragHandle(std::function<int()> nindex, QWidget& nrow);

 protected:
  virtual void mousePressEvent(QMouseEvent* event) override;
  virtual void mouseMoveEvent(QMouseEvent* event) override;

 private:
  const std::function<int()> index;
  QWidget& row;
  QPoint mpressed;
};

}  // namespace darknetxxQt6

#endif  // DARKNETXX_PROJECT_GUI_QT6_DRAGHANDLE_CLASS_H
