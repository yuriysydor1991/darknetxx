#include "src/gui/qt6/NetEditorMainWindow.h"

#include <QCloseEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <QPushButton>
#include <QScrollArea>
#include <QStatusBar>
#include <QToolBar>
#include <algorithm>
#include <sstream>

#include "project-global-decls.h"
#include "src/gui/net-editor/EditorIcons.h"
#include "src/gui/net-editor/SectionSchemas.h"
#include "src/gui/qt6/QtIcons.h"

namespace darknetxxQt6
{

namespace
{

/// @brief The heading label of a part of the window.
QLabel* heading(const QString& text)
{
  auto* label = new QLabel{text};
  QFont font = label->font();

  font.setBold(true);
  font.setPointSizeF(font.pointSizeF() * 1.25);
  label->setFont(font);

  return label;
}

/// @brief The dim description label of a part of the window.
QLabel* dim(const QString& text)
{
  auto* label = new QLabel{text};

  label->setWordWrap(true);
  label->setEnabled(false);

  return label;
}

/// @brief The network files the editor opens.
const QString NETWORKS_FILTER{
    "Network files (*.dxxwjz3p *.dxxwjz3 *.dxxwjz2p *.dxxwjz2);;All files (*)"};

/// @brief The projects the editor saves.
const QString PROJECTS_FILTER{
    "The dxxwjz3p projects (*.dxxwjz3p);;All files (*)"};

}  // namespace

NetEditorMainWindow::NetEditorMainWindow(const std::string& nname,
                                         const std::string& file)
    : mname{QString::fromStdString(nname + " " +
                                   project_decls::PROJECT_BUILD_VERSION)},
      mfile{new QLabel},
      mnetGrid{new QGridLayout},
      mnet{new QWidget},
      mclasses{new QPlainTextEdit},
      mlayers{new QVBoxLayout},
      mtype{new QComboBox},
      mtypeAbout{dim({})},
      madd{new QToolButton},
      mstatus{new QLabel}
{
  using net_editor::EditorIcons;

  static constexpr const int WIDTH = 1150;
  static constexpr const int HEIGHT = 850;
  static constexpr const int CLASSES_HEIGHT = 110;
  const QColor text = palette().color(QPalette::WindowText);
  auto* toolbar = addToolBar(QStringLiteral("File"));
  auto* scroll = new QScrollArea;
  auto* content = new QWidget;
  auto* layout = new QVBoxLayout{content};
  auto* add = new QHBoxLayout;

  const auto action = [&](const net_editor::EditorIcon& icon,
                          const QString& name, const QString& tip) {
    QAction* created = toolbar->addAction(QtIcons::of(icon, text), name);

    created->setToolTip(tip);

    return created;
  };

  toolbar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
  toolbar->setMovable(false);
  connect(action(EditorIcons::CREATE, QStringLiteral("&New"),
                 QStringLiteral("Start a new network")),
          &QAction::triggered, [this]() { create(); });
  connect(action(EditorIcons::OPEN, QStringLiteral("&Open…"),
                 QStringLiteral("Open the network of a dxxwjz3p project or of "
                                "another darknetxx file keeping one")),
          &QAction::triggered, [this]() { open(); });
  msave = action(EditorIcons::SAVE, QStringLiteral("&Save"),
                 QStringLiteral("Save the network into the opened file, ask "
                                "for the file of the new one"));
  msave->setShortcut(QKeySequence::Save);
  connect(msave, &QAction::triggered, [this]() { save(); });
  connect(action(EditorIcons::SAVE_AS, QStringLiteral("Save &as…"),
                 QStringLiteral("Save the network into another file")),
          &QAction::triggered, [this]() { save_as(); });
  mfile->setEnabled(false);
  mfile->setContentsMargins(12, 0, 12, 0);
  toolbar->addSeparator();
  toolbar->addWidget(mfile);

  for (const auto& type : net_editor::SectionSchemas::layer_types()) {
    mtype->addItem(QString::fromStdString(type));
  }

  mtype->setToolTip(QStringLiteral("The type of the layer to add"));
  madd->setIcon(QtIcons::of(EditorIcons::ADD, text));
  madd->setToolTip(
      QStringLiteral("Add the layer of the type at the bottom of the layers"));
  connect(mtype, &QComboBox::currentIndexChanged, [this](int index) {
    const auto& types = net_editor::SectionSchemas::layer_types();

    mtypeAbout->setText(
        index >= 0 && static_cast<std::size_t>(index) < types.size()
            ? QString::fromStdString(net_editor::SectionSchemas::of(
                                         types[static_cast<std::size_t>(index)])
                                         .about())
            : QString{});
  });
  connect(madd, &QToolButton::clicked,
          [this]() { add_layer(mtype->currentText().toStdString()); });
  mtypeAbout->setText(QString::fromStdString(
      net_editor::SectionSchemas::of(mtype->currentText().toStdString())
          .about()));

  mnet->setLayout(mnetGrid);
  mclasses->setFixedHeight(CLASSES_HEIGHT);
  mclasses->setFont(QFont{QStringLiteral("monospace")});
  connect(mclasses, &QPlainTextEdit::textChanged, [this]() {
    if (mshowing) {
      return;
    }

    std::istringstream lines{mclasses->toPlainText().toStdString()};
    std::string name;
    auto& classes = mproject.model.classes;

    classes.clear();

    // the empty lines name no class
    while (std::getline(lines, name)) {
      if (!name.empty()) {
        classes.push_back(name);
      }
    }

    changed();
  });

  mlayers->setSpacing(2);
  add->addWidget(mtype);
  add->addWidget(madd);
  add->addWidget(mtypeAbout, 1);

  layout->setContentsMargins(12, 12, 12, 12);
  layout->addWidget(heading(QStringLiteral("Network")));
  layout->addWidget(mnet);
  layout->addWidget(heading(QStringLiteral("Classes")));
  layout->addWidget(dim(QStringLiteral(
      "The names of the classes the network detects, a name on a line: the "
      "classes= of the detection layers is their count")));
  layout->addWidget(mclasses);
  layout->addWidget(heading(QStringLiteral("Layers")));
  layout->addWidget(dim(QStringLiteral(
      "Drag a layer by it's handle to move it, the references of the [route] "
      "and the [shortcut] layers follow the moved layers")));
  layout->addLayout(mlayers);
  layout->addLayout(add);
  layout->addStretch();

  scroll->setWidget(content);
  scroll->setWidgetResizable(true);
  scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setCentralWidget(scroll);

  mstatus->setWordWrap(true);
  mstatus->setTextInteractionFlags(Qt::TextSelectableByMouse);
  statusBar()->addWidget(mstatus, 1);
  resize(WIDTH, HEIGHT);

  if (!file.empty() && open_file(file)) {
    return;
  }

  if (file.empty()) {
    tell("The new network");
  }

  show_network();
}

bool NetEditorMainWindow::open_file(const std::string& path)
{
  std::string message;

  if (!mproject.open(path, message)) {
    tell(message, true);
    return false;
  }

  tell("Opened the " + path);
  show_network();

  return true;
}

bool NetEditorMainWindow::save_into(const std::string& path)
{
  std::string message;
  const bool saved = mproject.save(path, message);

  tell(saved
           ? "Saved into the " + path + (message.empty() ? "" : ". " + message)
           : message,
       !saved);
  show_title();

  return saved;
}

void NetEditorMainWindow::add_layer(const std::string& type)
{
  append(
      mproject.model.add(type, net_editor::SectionSchemas::of(type).initial()))
      ->expand(true);
  changed();
}

void NetEditorMainWindow::move_layer(const int from, const int to)
{
  if (from == to || from < 0 || to < 0) {
    return;
  }

  LayerRowWidget* moved = mrows[static_cast<std::size_t>(from)];

  mproject.model.move(from, to);
  mrows.erase(mrows.begin() + from);
  mrows.insert(mrows.begin() + to, moved);
  mlayers->removeWidget(moved);
  mlayers->insertWidget(to, moved);
  changed();
}

net_editor::NetworkProject& NetEditorMainWindow::project() { return mproject; }

QtOptionsGrid& NetEditorMainWindow::net_rows() { return *mnetRows; }

const std::vector<LayerRowWidget*>& NetEditorMainWindow::rows() const
{
  return mrows;
}

QPlainTextEdit& NetEditorMainWindow::classes_edit() { return *mclasses; }

QComboBox& NetEditorMainWindow::layer_type_combo() { return *mtype; }

QToolButton& NetEditorMainWindow::add_layer_button() { return *madd; }

QLabel& NetEditorMainWindow::status_label() { return *mstatus; }

QAction& NetEditorMainWindow::save_action() { return *msave; }

void NetEditorMainWindow::closeEvent(QCloseEvent* event)
{
  if (confirm("exit")) {
    event->accept();
  } else {
    event->ignore();
  }
}

void NetEditorMainWindow::create()
{
  if (confirm("start the new one")) {
    mproject.create();
    tell("The new network");
    show_network();
  }
}

void NetEditorMainWindow::open()
{
  if (!confirm("open another one")) {
    return;
  }

  const QString path = QFileDialog::getOpenFileName(
      this, QStringLiteral("Open the network"), {}, NETWORKS_FILTER);

  if (!path.isEmpty()) {
    open_file(path.toStdString());
  }
}

void NetEditorMainWindow::save()
{
  if (mproject.path().empty()) {
    save_as();
  } else {
    save_into(mproject.path());
  }
}

bool NetEditorMainWindow::save_as()
{
  const std::string& current = mproject.path();
  const QString chosen = QFileDialog::getSaveFileName(
      this, QStringLiteral("Save the network as the project"),
      QString::fromStdString(
          current.empty() ? "network" + net_editor::NetworkProject::EXTENSION
                          : current),
      PROJECTS_FILTER);

  return !chosen.isEmpty() && save_into(net_editor::NetworkProject::project_of(
                                  chosen.toStdString()));
}

bool NetEditorMainWindow::confirm(const std::string& name)
{
  if (!mproject.modified()) {
    return true;
  }

  QMessageBox prompt{QMessageBox::Question,
                     QStringLiteral("The network has changes of no save"),
                     QStringLiteral("The network has changes of no save"),
                     QMessageBox::NoButton, this};
  const QString action = QString::fromStdString(name);
  auto* save = prompt.addButton(
      mproject.path().empty() ? QStringLiteral("&Save as… and %1").arg(action)
                              : QStringLiteral("&Save and %1").arg(action),
      QMessageBox::AcceptRole);
  auto* cancel = prompt.addButton(QMessageBox::Cancel);
  auto* discard = prompt.addButton(QStringLiteral("&Discard the changes"),
                                   QMessageBox::DestructiveRole);

  prompt.setInformativeText(
      QStringLiteral("Save them to %1, or discard them?").arg(action));
  prompt.setDefaultButton(save);
  prompt.setEscapeButton(cancel);
  prompt.exec();

  if (prompt.clickedButton() == discard) {
    return true;
  }

  if (prompt.clickedButton() != save) {
    return false;
  }

  return mproject.path().empty() ? save_as() : save_into(mproject.path());
}

void NetEditorMainWindow::remove_layer(LayerRowWidget& row)
{
  const auto found = std::find(mrows.begin(), mrows.end(), &row);

  mproject.model.remove(static_cast<int>(found - mrows.begin()));
  mrows.erase(found);
  mlayers->removeWidget(&row);

  // the row of the clicked button goes once the click is handled
  row.hide();
  row.deleteLater();
  changed();
}

LayerRowWidget* NetEditorMainWindow::append(
    const net_editor::NetworkSectionPtr& section)
{
  auto* row = new LayerRowWidget{
      mproject.model, section,
      LayerRowWidget::Handlers{
          [this]() { changed(); },
          [this](LayerRowWidget& removed) { remove_layer(removed); },
          [this](int from, int to) { move_layer(from, to); }}};

  mlayers->addWidget(row);
  mrows.push_back(row);

  return row;
}

void NetEditorMainWindow::show_network()
{
  const auto& model = mproject.model;
  QString classes;

  for (auto* row : mrows) {
    delete row;
  }

  mrows.clear();

  // the widgets of the rows go before the rows
  while (QLayoutItem* item = mnetGrid->takeAt(0)) {
    delete item->widget();
    delete item;
  }

  mnetRows = std::make_unique<QtOptionsGrid>(*mnetGrid, model, model.net(),
                                             [this]() { changed(); });

  for (const auto& name : model.classes) {
    classes += QString::fromStdString(name) + "\n";
  }

  mshowing = true;
  mclasses->setPlainText(classes);
  mshowing = false;

  for (const auto& layer : model.layers()) {
    append(layer);
  }

  show_title();
}

void NetEditorMainWindow::changed()
{
  mnetRows->refresh();

  for (auto* row : mrows) {
    row->refresh();
  }

  show_title();
}

void NetEditorMainWindow::show_title()
{
  const QString title = QString::fromStdString(mproject.title());

  setWindowTitle(mname + " - " + title);
  mfile->setText(title);
  mfile->setToolTip(QString::fromStdString(mproject.path()));
}

void NetEditorMainWindow::tell(const std::string& message, const bool failure)
{
  mstatus->setText(QString::fromStdString(message));
  mstatus->setStyleSheet(failure ? QStringLiteral("color: #e01b24;")
                                 : QString{});
}

}  // namespace darknetxxQt6
