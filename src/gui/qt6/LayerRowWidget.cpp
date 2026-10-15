#include "src/gui/qt6/LayerRowWidget.h"

#include <QDragEnterEvent>
#include <QDropEvent>
#include <QHBoxLayout>
#include <QMimeData>
#include <QVBoxLayout>

#include "src/gui/net-editor/EditorIcons.h"
#include "src/gui/net-editor/OptionPresenter.h"
#include "src/gui/net-editor/SectionSchemas.h"
#include "src/gui/qt6/DragHandle.h"
#include "src/gui/qt6/QtIcons.h"

namespace darknetxxQt6
{

LayerRowWidget::LayerRowWidget(const net_editor::NetworkModel& nmodel,
                               net_editor::NetworkSectionPtr nsection,
                               Handlers nhandlers)
    : model{nmodel},
      msection{std::move(nsection)},
      handlers{std::move(nhandlers)},
      mindex{new QLabel},
      mexpand{new QToolButton},
      msummary{new QLabel},
      merrors{new QLabel},
      mremove{new QToolButton},
      mbody{new QWidget},
      mgrid{new QGridLayout}
{
  static constexpr const int HANDLE_SIZE = 16;
  static constexpr const int INDEX_WIDTH = 40;
  auto* layout = new QVBoxLayout{this};
  auto* header = new QHBoxLayout;
  auto* handle =
      new DragHandle{[this]() { return model.index_of(*msection); }, *this};
  auto* type = new QLabel{
      QStringLiteral("<b>[%1]</b>")
          .arg(QString::fromStdString(msection->type()).toHtmlEscaped())};
  auto* about = new QLabel{QString::fromStdString(
      net_editor::SectionSchemas::of(msection->type()).about())};
  auto* bodyLayout = new QVBoxLayout{mbody};
  const QColor text = palette().color(QPalette::WindowText);

  setFrameShape(QFrame::StyledPanel);
  setAcceptDrops(true);

  handle->setPixmap(QtIcons::of(net_editor::EditorIcons::DRAG, text)
                        .pixmap(HANDLE_SIZE, HANDLE_SIZE));
  mindex->setFixedWidth(INDEX_WIDTH);
  mindex->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
  mexpand->setArrowType(Qt::RightArrow);
  mexpand->setAutoRaise(true);
  mexpand->setCheckable(true);
  mexpand->setToolTip(QStringLiteral("Show the options of the layer"));
  msummary->setEnabled(false);
  msummary->setMinimumWidth(1);
  msummary->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Preferred);
  merrors->setStyleSheet(QStringLiteral("color: #e01b24;"));
  mremove->setIcon(QtIcons::of(net_editor::EditorIcons::REMOVE, text));
  mremove->setAutoRaise(true);
  mremove->setToolTip(QStringLiteral("Remove the layer"));

  header->addWidget(handle);
  header->addWidget(mindex);
  header->addWidget(mexpand);
  header->addWidget(type);
  header->addWidget(msummary, 1);
  header->addWidget(merrors);
  header->addWidget(mremove);

  about->setWordWrap(true);
  about->setEnabled(false);
  bodyLayout->setContentsMargins(INDEX_WIDTH, 0, 0, 0);
  bodyLayout->addWidget(about);
  bodyLayout->addLayout(mgrid);
  mbody->setVisible(false);

  layout->setContentsMargins(4, 2, 4, 2);
  layout->addLayout(header);
  layout->addWidget(mbody);

  QObject::connect(mexpand, &QToolButton::toggled,
                   [this](bool expanded) { expand(expanded); });
  QObject::connect(mremove, &QToolButton::clicked,
                   [this]() { handlers.removed(*this); });

  refresh();
}

void LayerRowWidget::refresh()
{
  std::size_t wrong = 0U;
  std::string first;

  for (const auto& row : net_editor::OptionPresenter::of(model, msection)) {
    const std::string error = row->error();

    if (!error.empty() && wrong++ == 0U) {
      first = row->key() + ": " + error;
    }
  }

  mindex->setText(QString::number(model.index_of(*msection)));
  msummary->setText(QString::fromStdString(msection->summary()));
  merrors->setText(wrong == 0U ? QString{}
                               : QStringLiteral("⚠ %1 wrong").arg(wrong));
  merrors->setToolTip(QString::fromStdString(first));

  if (moptions != nullptr) {
    moptions->refresh();
  }
}

void LayerRowWidget::expand(const bool expanded)
{
  if (moptions == nullptr && expanded) {
    moptions = std::make_unique<QtOptionsGrid>(*mgrid, model, msection,
                                               handlers.edited);
  }

  if (mexpand->isChecked() != expanded) {
    mexpand->setChecked(expanded);
  }

  mexpand->setArrowType(expanded ? Qt::DownArrow : Qt::RightArrow);
  mbody->setVisible(expanded);
}

const net_editor::NetworkSectionPtr& LayerRowWidget::section() const
{
  return msection;
}

QToolButton& LayerRowWidget::expand_button() { return *mexpand; }

QToolButton& LayerRowWidget::remove_button() { return *mremove; }

QLabel& LayerRowWidget::index_label() { return *mindex; }

QLabel& LayerRowWidget::summary_label() { return *msummary; }

QLabel& LayerRowWidget::errors_label() { return *merrors; }

QtOptionsGrid* LayerRowWidget::options() { return moptions.get(); }

void LayerRowWidget::dragEnterEvent(QDragEnterEvent* event)
{
  if (event->mimeData()->hasFormat(DragHandle::MIME)) {
    event->acceptProposedAction();
  }
}

void LayerRowWidget::dropEvent(QDropEvent* event)
{
  bool number = false;
  const int from = event->mimeData()->data(DragHandle::MIME).toInt(&number);

  if (number) {
    event->acceptProposedAction();
    handlers.moved(from, model.index_of(*msection));
  }
}

}  // namespace darknetxxQt6
