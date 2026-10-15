#include "src/gui/qt6/QtIcons.h"

#include <QFile>
#include <QPainter>
#include <QPixmap>
#include <QSvgRenderer>

namespace darknetxxQt6
{

QIcon QtIcons::of(const net_editor::EditorIcon& icon, const QColor& color)
{
#ifdef DARKNETXX_FONTAWESOME
  static constexpr const int SIZE = 64;
  QFile file{QStringLiteral(":/icons/solid/%1.svg").arg(icon.awesome)};

  if (!file.open(QIODevice::ReadOnly)) {
    return {};
  }

  // the currentColor of the paths is the color of the text
  QByteArray svg = file.readAll();

  svg.replace("<svg ", "<svg color=\"" + color.name().toUtf8() + "\" ");

  QSvgRenderer renderer{svg};
  const QSize size =
      renderer.defaultSize().scaled(SIZE, SIZE, Qt::KeepAspectRatio);
  QPixmap pixmap{SIZE, SIZE};

  pixmap.fill(Qt::transparent);

  QPainter painter{&pixmap};

  renderer.render(&painter, QRectF{(SIZE - size.width()) / 2.0,
                                   (SIZE - size.height()) / 2.0,
                                   static_cast<qreal>(size.width()),
                                   static_cast<qreal>(size.height())});

  return QIcon{pixmap};
#else
  (void)color;
  return QIcon::fromTheme(QString::fromUtf8(icon.theme));
#endif  // DARKNETXX_FONTAWESOME
}

}  // namespace darknetxxQt6
