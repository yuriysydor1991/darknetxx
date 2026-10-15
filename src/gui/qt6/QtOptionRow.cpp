#include "src/gui/qt6/QtOptionRow.h"

#include <QVBoxLayout>
#include <algorithm>

#include "src/gui/qt6/OptionValidator.h"

namespace darknetxxQt6
{

QtOptionRow::QtOptionRow(QGridLayout& grid, const int row,
                         net_editor::OptionPresenterPtr npresenter,
                         std::function<void()> edited)
    : mpresenter{std::move(npresenter)}
{
  static constexpr const int EDITOR_WIDTH = 220;
  auto* key = new QLabel{QString::fromStdString(mpresenter->key())};
  auto* about = new QWidget;
  auto* aboutLayout = new QVBoxLayout{about};
  const std::string value = mpresenter->value();
  QFont monospace = key->font();

  monospace.setFamily(QStringLiteral("monospace"));
  key->setFont(monospace);
  key->setTextInteractionFlags(Qt::TextSelectableByMouse);

  mchoices = mpresenter->choices();

  if (mchoices.empty()) {
    mline = new QLineEdit{QString::fromStdString(value)};
    mline->setFixedWidth(EDITOR_WIDTH);
    mline->setValidator(new OptionValidator{mpresenter, mline});
    QObject::connect(mline, &QLineEdit::textChanged,
                     [this, edited](const QString& text) {
                       mpresenter->value(text.toStdString());
                       edited();
                     });
    grid.addWidget(mline, row, 1, Qt::AlignTop);
  } else {
    mchoices.insert(mchoices.begin(), std::string{});

    // the wrong value of the file stays to be told
    if (std::find(mchoices.begin(), mchoices.end(), value) == mchoices.end()) {
      mchoices.push_back(value);
    }

    mcombo = new QComboBox;

    for (const auto& choice : mchoices) {
      mcombo->addItem(QString::fromStdString(
          choice.empty() ? mpresenter->placeholder() : choice));
    }

    mcombo->setCurrentIndex(static_cast<int>(
        std::find(mchoices.begin(), mchoices.end(), value) - mchoices.begin()));
    mcombo->setFixedWidth(EDITOR_WIDTH);
    QObject::connect(
        mcombo, &QComboBox::currentIndexChanged, [this, edited](int index) {
          mpresenter->value(index >= 0 && static_cast<std::size_t>(index) <
                                              mchoices.size()
                                ? mchoices[static_cast<std::size_t>(index)]
                                : std::string{});
          edited();
        });
    grid.addWidget(mcombo, row, 1, Qt::AlignTop);
  }

  mdescription = new QLabel;
  mdescription->setWordWrap(true);
  mdescription->setEnabled(false);
  merror = new QLabel;
  merror->setWordWrap(true);
  merror->setStyleSheet(QStringLiteral("color: #e01b24;"));
  aboutLayout->setContentsMargins(0, 0, 0, 0);
  aboutLayout->addWidget(mdescription);
  aboutLayout->addWidget(merror);

  grid.addWidget(key, row, 0, Qt::AlignTop);
  grid.addWidget(about, row, 2);

  refresh();
}

QtOptionRow::~QtOptionRow()
{
  for (QWidget* editor :
       {static_cast<QWidget*>(mline), static_cast<QWidget*>(mcombo)}) {
    if (editor != nullptr) {
      editor->blockSignals(true);
    }
  }
}

void QtOptionRow::refresh()
{
  const std::string error = mpresenter->error();

  mdescription->setText(QString::fromStdString(mpresenter->description()));
  merror->setText(QString::fromStdString(error));
  merror->setVisible(!error.empty());

  if (mline != nullptr) {
    mline->setPlaceholderText(
        QString::fromStdString(mpresenter->placeholder()));
    mline->setStyleSheet(error.empty()
                             ? QString{}
                             : QStringLiteral("border: 1px solid #e01b24;"));
  }
}

const net_editor::OptionPresenter& QtOptionRow::presenter() const
{
  return *mpresenter;
}

QLineEdit* QtOptionRow::line() const { return mline; }

QComboBox* QtOptionRow::combo() const { return mcombo; }

QLabel& QtOptionRow::description() const { return *mdescription; }

QLabel& QtOptionRow::error() const { return *merror; }

}  // namespace darknetxxQt6
