#include <gtest/gtest.h>

#include <QAbstractButton>
#include <QApplication>
#include <QMessageBox>
#include <QTimer>
#include <filesystem>
#include <memory>
#include <string>

#include "src/gui/net-editor/NetworkProject.h"
#include "src/gui/net-editor/SectionSchemas.h"
#include "src/gui/qt6/NetEditorMainWindow.h"

using darknetxxQt6::LayerRowWidget;
using darknetxxQt6::NetEditorMainWindow;
using darknetxxQt6::QtOptionRow;

namespace fs = std::filesystem;

class CTEST_NetEditorQtWindow : public testing::Test
{
 public:
  void SetUp() override
  {
    static int argc = 1;
    static char name[] = "CTEST_NetEditorQtWindow";
    static char* argv[] = {name, nullptr};

    // the window of no display
    if (qEnvironmentVariableIsEmpty("QT_QPA_PLATFORM")) {
      qputenv("QT_QPA_PLATFORM", "offscreen");
    }

    static QApplication app{argc, argv};

    fs::remove_all(dir);
    fs::create_directories(dir);
  }

  void TearDown() override
  {
    window.reset();
    fs::remove_all(dir);
  }

  std::string file(const std::string& name) const
  {
    return (dir / name).string();
  }

  /// @brief The project of the convolutional, the maxpool and the route
  /// layers.
  std::string project()
  {
    net_editor::NetworkProject made;
    std::string message;

    for (const std::string type : {"convolutional", "maxpool", "route"}) {
      made.model.add(type, net_editor::SectionSchemas::of(type).initial());
    }

    made.model.layers()[2]->set("layers", "-1,0");
    made.model.layers()[1]->set("stride", "1");
    made.model.classes = {"cat"};

    EXPECT_TRUE(made.save(file("net.dxxwjz3p"), message)) << message;

    return file("net.dxxwjz3p");
  }

  void show(const std::string& project = "")
  {
    window = std::make_unique<NetEditorMainWindow>(
        "darknetxx-net-editor-qt-gui", project);
    window->show();
    QApplication::processEvents();
  }

  QtOptionRow& net_row(const std::string& key)
  {
    for (const auto& row : window->net_rows().rows()) {
      if (row->presenter().key() == key) {
        return *row;
      }
    }

    throw std::runtime_error{"No row of the " + key};
  }

  const std::vector<LayerRowWidget*>& rows() { return window->rows(); }

  /// @brief Clicks the button of the text of the prompt the close shows.
  void answer(const QString& text)
  {
    QTimer::singleShot(0, [text]() {
      auto* prompt =
          qobject_cast<QMessageBox*>(QApplication::activeModalWidget());

      ASSERT_NE(prompt, nullptr);

      for (auto* button : prompt->buttons()) {
        if (button->text().contains(text)) {
          button->click();
          return;
        }
      }

      FAIL() << "No " << text.toStdString() << " button";
    });
  }

  std::string status() { return window->status_label().text().toStdString(); }

  fs::path dir{fs::path{CTEST_NetEditorQtWindow_DATA_DIR} /
               testing::UnitTest::GetInstance()->current_test_info()->name()};
  std::unique_ptr<NetEditorMainWindow> window;
};

TEST_F(CTEST_NetEditorQtWindow, the_new_network_is_shown_at_first)
{
  show();

  EXPECT_EQ(status(), "The new network");
  EXPECT_TRUE(rows().empty());
  EXPECT_TRUE(window->windowTitle().endsWith(QStringLiteral(" - New network")))
      << window->windowTitle().toStdString();
  EXPECT_EQ(net_row("batch").line()->text(), QStringLiteral("64"));
  EXPECT_EQ(net_row("policy").combo()->currentText(), QStringLiteral("steps"));
  EXPECT_EQ(net_row("hue").line()->placeholderText(),
            QStringLiteral("default: 0"));
}

TEST_F(CTEST_NetEditorQtWindow, the_project_of_the_command_line_is_opened)
{
  show(project());

  ASSERT_EQ(rows().size(), 3U);
  EXPECT_EQ(rows()[2]->summary_label().text(), QStringLiteral("layers=-1,0"));
  EXPECT_EQ(window->classes_edit().toPlainText(), QStringLiteral("cat\n"));
  EXPECT_FALSE(window->project().modified());
}

TEST_F(CTEST_NetEditorQtWindow, the_entries_tell_the_limits_of_the_moment)
{
  show();

  auto& subdivisions = net_row("subdivisions");

  subdivisions.line()->setText(QStringLiteral("7"));

  EXPECT_FALSE(subdivisions.error().isHidden());
  EXPECT_EQ(subdivisions.error().text(),
            QStringLiteral("The 7 doesn't divide the batch=64"));

  net_row("batch").line()->setText(QStringLiteral("63"));

  EXPECT_TRUE(subdivisions.error().isHidden());
  EXPECT_TRUE(subdivisions.description().text().contains(
      QStringLiteral("a divisor of the batch=63")));
  EXPECT_TRUE(window->windowTitle().endsWith(QStringLiteral("New network *")));
}

TEST_F(CTEST_NetEditorQtWindow, the_entries_take_the_characters_of_the_values)
{
  show();

  auto* batch = net_row("batch").line();

  batch->setFocus();
  batch->end(false);
  batch->insert(QStringLiteral("x"));
  batch->insert(QStringLiteral("1"));

  EXPECT_EQ(batch->text(), QStringLiteral("641"));
  EXPECT_EQ(*window->project().model.net()->find("batch"), "641");
}

TEST_F(CTEST_NetEditorQtWindow, the_layers_are_added_moved_and_removed)
{
  show(project());

  window->layer_type_combo().setCurrentText(QStringLiteral("upsample"));
  window->add_layer_button().click();

  ASSERT_EQ(rows().size(), 4U);
  EXPECT_NE(rows()[3]->options(), nullptr);
  EXPECT_EQ(window->project().model.layers()[3]->type(), "upsample");

  // the route dropped onto the place of the maxpool
  LayerRowWidget* route = rows()[2];

  window->move_layer(2, 1);

  EXPECT_EQ(rows()[1], route);
  EXPECT_EQ(route->index_label().text(), QStringLiteral("1"));
  EXPECT_EQ(route->summary_label().text(), QStringLiteral("layers=2,0"));
  EXPECT_EQ(route->errors_label().text(), QStringLiteral("⚠ 1 wrong"));

  route->remove_button().click();
  QApplication::processEvents();

  ASSERT_EQ(rows().size(), 3U);
  EXPECT_EQ(rows()[1]->index_label().text(), QStringLiteral("1"));
  EXPECT_EQ(window->project().model.layers()[1]->type(), "maxpool");
}

TEST_F(CTEST_NetEditorQtWindow, the_network_is_saved_into_the_opened_project)
{
  show(project());

  net_row("max_batches").line()->setText(QStringLiteral("8000"));
  window->save_action().trigger();

  EXPECT_EQ(status(), "Saved into the " + file("net.dxxwjz3p"));
  EXPECT_FALSE(window->project().modified());

  net_editor::NetworkProject saved;
  std::string message;

  ASSERT_TRUE(saved.open(file("net.dxxwjz3p"), message));
  EXPECT_EQ(*saved.model.net()->find("max_batches"), "8000");
}

TEST_F(CTEST_NetEditorQtWindow, the_wrong_network_is_not_saved)
{
  show(project());

  net_row("subdivisions").line()->setText(QStringLiteral("7"));
  window->save_action().trigger();

  EXPECT_EQ(status(),
            "The network has 1 wrong options:\n[net] subdivisions: The 7 "
            "doesn't divide the batch=64");
  EXPECT_TRUE(window->project().modified());
}

TEST_F(CTEST_NetEditorQtWindow, the_changed_network_asks_before_the_exit)
{
  show(project());

  net_row("max_batches").line()->setText(QStringLiteral("8000"));

  answer(QStringLiteral("Cancel"));

  EXPECT_FALSE(window->close());
  EXPECT_TRUE(window->isVisible());

  answer(QStringLiteral("Save and exit"));

  EXPECT_TRUE(window->close());
  EXPECT_FALSE(window->project().modified());
}

TEST_F(CTEST_NetEditorQtWindow, the_discarded_changes_close_the_window)
{
  show(project());

  net_row("max_batches").line()->setText(QStringLiteral("8000"));
  answer(QStringLiteral("Discard"));

  EXPECT_TRUE(window->close());

  net_editor::NetworkProject saved;
  std::string message;

  ASSERT_TRUE(saved.open(file("net.dxxwjz3p"), message));
  EXPECT_EQ(*saved.model.net()->find("max_batches"), "6000");
}

TEST_F(CTEST_NetEditorQtWindow, the_classes_are_the_lines_of_names)
{
  show();

  window->classes_edit().setPlainText(QStringLiteral("cat\n\ndog\n"));

  EXPECT_EQ(window->project().model.classes,
            (std::vector<std::string>{"cat", "dog"}));
  EXPECT_TRUE(window->project().modified());
}
