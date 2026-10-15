#include "src/gui/qt6/DragHandle.h"

#include <QApplication>
#include <QDrag>
#include <QMimeData>
#include <QMouseEvent>

namespace darknetxxQt6
{

DragHandle::DragHandle(std::function<int()> nindex, QWidget& nrow)
    : index{std::move(nindex)}, row{nrow}
{
  setCursor(Qt::OpenHandCursor);
  setToolTip(QStringLiteral("Drag the layer onto the place of another one"));
}

void DragHandle::mousePressEvent(QMouseEvent* event)
{
  if (event->button() == Qt::LeftButton) {
    mpressed = event->position().toPoint();
  }

  QLabel::mousePressEvent(event);
}

void DragHandle::mouseMoveEvent(QMouseEvent* event)
{
  if (!(event->buttons() & Qt::LeftButton) ||
      (event->position().toPoint() - mpressed).manhattanLength() <
          QApplication::startDragDistance()) {
    QLabel::mouseMoveEvent(event);
    return;
  }

  auto* mime = new QMimeData;
  auto* drag = new QDrag{this};

  mime->setData(MIME, QByteArray::number(index()));
  drag->setMimeData(mime);
  drag->setPixmap(row.grab());
  drag->setHotSpot(mpressed);
  drag->exec(Qt::MoveAction);
}

}  // namespace darknetxxQt6
