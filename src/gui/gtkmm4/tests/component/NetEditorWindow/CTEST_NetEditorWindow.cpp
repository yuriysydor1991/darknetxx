#include <glibmm/main.h>
#include <gtest/gtest.h>
#include <gtk/gtk.h>
#include <gtkmm/init.h>

#include <chrono>
#include <filesystem>
#include <functional>
#include <memory>
#include <string>
#include <thread>

#include "LibraryFacade.h"
#include "src/app/ApplicationContext.h"
#include "src/gui/gtkmm4/net-editor-window/NetEditorWindowContext.h"
#include "src/gui/gtkmm4/net-editor-window/NetworkEventsHandler.h"
#include "src/gui/gtkmm4/net-editor-window/ProjectEventsHandler.h"
#include "src/gui/net-editor/NetworkProject.h"
#include "src/gui/net-editor/SectionSchemas.h"

using darknetxxGtkmm4::NetEditorWindowContext;
using darknetxxGtkmm4::window::event_handlers::ProjectEventsHandler;
using darknetxxGtkmm4::window::widgets::LayerRow;
using darknetxxGtkmm4::window::widgets::OptionRow;

namespace fs = std::filesystem;

class CTEST_NetEditorWindow : public testing::Test
{
 public:
  void SetUp() override
  {
    static const bool gtkReady = gtk_init_check();

    if (!gtkReady) {
      GTEST_SKIP() << "No display to show the window on";
    }

    static const bool gtkmmReady = []() {
      Gtk::init_gtkmm_internals();
      return true;
    }();

    ASSERT_TRUE(gtkmmReady);

    fs::remove_all(dir);
    fs::create_directories(dir);
  }

  void TearDown() override
  {
    if (mwctx != nullptr) {
      mwctx->clear();
    }

    fs::remove_all(dir);
  }

  std::string file(const std::string& name) const
  {
    return (dir / name).string();
  }

  /// @brief The project of the convolutional and the maxpool layers.
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

  /// @brief Shows the window of the project of the command line.
  void show(const std::string& project = "")
  {
    libctx->train_cfg = project;
    mwctx = std::make_shared<NetEditorWindowContext>(actx, libctx);
    loader = &mwctx->wloader;

    ASSERT_TRUE(mwctx->load());

    loader->window()->present();
    iterate();
  }

  /// @brief Runs the pending events of the main loop.
  static void iterate()
  {
    while (Glib::MainContext::get_default()->iteration(false)) {
    }
  }

  /// @brief Runs the main loop for a while, the folder reads of the file
  /// chooser end then.
  static void settle()
  {
    const auto end =
        std::chrono::steady_clock::now() + std::chrono::milliseconds{300};

    wait_for([end]() { return std::chrono::steady_clock::now() > end; });
  }

  /// @brief Runs the main loop till the condition holds or the time is out.
  static bool wait_for(const std::function<bool()>& condition)
  {
    const auto end = std::chrono::steady_clock::now() + std::chrono::minutes{1};

    while (!condition()) {
      if (std::chrono::steady_clock::now() > end) {
        return false;
      }

      Glib::MainContext::get_default()->iteration(false);
      std::this_thread::sleep_for(std::chrono::milliseconds{5});
    }

    return true;
  }

  /// @brief The row of the option of the [net] section.
  OptionRow& net_row(const std::string& key)
  {
    for (const auto& row : mwctx->network->net_rows().rows()) {
      if (row->presenter().key() == key) {
        return *row;
      }
    }

    throw std::runtime_error{"No row of the " + key};
  }

  const std::vector<LayerRow*>& rows() { return mwctx->network->rows(); }

  /// @brief Adds the layer of the type by the + button.
  void add(const std::string& type)
  {
    const auto& types = net_editor::SectionSchemas::layer_types();

    loader->layer_type_dropdown()->set_selected(static_cast<guint>(
        std::find(types.begin(), types.end(), type) - types.begin()));
    click(*loader->add_layer_button());
  }

  /// @brief The click of the button, of no press animation.
  static void click(Gtk::Button& button)
  {
    g_signal_emit_by_name(button.gobj(), "clicked");
  }

  std::string status() { return loader->status_label()->get_text().raw(); }

  std::string title() { return loader->window()->get_title().raw(); }

  fs::path dir{fs::path{CTEST_NetEditorWindow_DATA_DIR} /
               testing::UnitTest::GetInstance()->current_test_info()->name()};
  int argc{0};
  char** argv{nullptr};
  std::shared_ptr<app::ApplicationContext> actx{
      std::make_shared<app::ApplicationContext>(argc, argv)};
  darknetxx011::LibraryContextPtr libctx{
      darknetxx011::LibraryFacade::create_library_context()};
  std::shared_ptr<NetEditorWindowContext> mwctx;
  darknetxxGtkmm4::window::NetEditorWindowLoader* loader{nullptr};
};

TEST_F(CTEST_NetEditorWindow, the_new_network_is_shown_at_first)
{
  show();

  EXPECT_EQ(status(), "The new network");
  EXPECT_TRUE(rows().empty());
  EXPECT_EQ(loader->file_label()->get_text().raw(), "New network");
  EXPECT_NE(title().find("New network"), std::string::npos) << title();

  // the static table of the [net] options
  EXPECT_EQ(net_row("batch").entry()->get_text().raw(), "64");
  EXPECT_EQ(net_row("policy").dropdown()->get_selected(), 2U);
  EXPECT_EQ(net_row("flip").entry(), nullptr);
  EXPECT_EQ(net_row("hue").entry()->get_placeholder_text().raw(), "default: 0");
  EXPECT_NE(net_row("subdivisions")
                .description()
                .get_text()
                .raw()
                .find("a divisor of the batch=64"),
            std::string::npos);
}

TEST_F(CTEST_NetEditorWindow, the_project_of_the_command_line_is_opened)
{
  show(project());

  EXPECT_EQ(status(), "Opened the " + file("net.dxxwjz3p"));
  ASSERT_EQ(rows().size(), 3U);
  EXPECT_EQ(rows()[0]->summary_label().get_text().raw(),
            "batch_normalize=1 filters=32 size=3 stride=1 pad=1 "
            "activation=leaky");
  EXPECT_EQ(rows()[2]->index_label().get_text().raw(), "2");
  EXPECT_EQ(loader->classes_view()->get_buffer()->get_text().raw(), "cat\n");
  EXPECT_FALSE(mwctx->project.modified());
}

TEST_F(CTEST_NetEditorWindow, the_entries_tell_the_limits_of_the_moment)
{
  show();

  auto& subdivisions = net_row("subdivisions");

  subdivisions.entry()->set_text("7");

  EXPECT_TRUE(subdivisions.error().get_visible());
  EXPECT_EQ(subdivisions.error().get_text().raw(),
            "The 7 doesn't divide the batch=64");
  EXPECT_TRUE(subdivisions.entry()->has_css_class("error"));

  // the batch changes the limits of the subdivisions
  net_row("batch").entry()->set_text("63");

  EXPECT_FALSE(subdivisions.error().get_visible());
  EXPECT_NE(subdivisions.description().get_text().raw().find(
                "from 1 to 63, a divisor of the batch=63"),
            std::string::npos);
  EXPECT_NE(title().find("New network *"), std::string::npos) << title();
}

TEST_F(CTEST_NetEditorWindow, the_entries_keep_the_characters_of_the_values)
{
  show();

  auto* batch = net_row("batch").entry();
  int position = 2;

  // the typed characters of no integer don't get in
  batch->insert_text("x", 1, position);
  batch->insert_text("1", 1, position);

  EXPECT_EQ(batch->get_text().raw(), "641");
  EXPECT_EQ(*mwctx->project.model.net()->find("batch"), "641");

  // the cleared entry takes the default
  batch->set_text("");

  EXPECT_EQ(mwctx->project.model.net()->find("batch"), nullptr);
  EXPECT_EQ(batch->get_placeholder_text().raw(), "default: 1");
}

TEST_F(CTEST_NetEditorWindow, the_layers_are_added_and_removed)
{
  show();

  add("convolutional");
  add("yolo");

  ASSERT_EQ(rows().size(), 2U);
  EXPECT_TRUE(rows()[1]->expander().get_expanded());
  ASSERT_NE(rows()[1]->options(), nullptr);
  EXPECT_EQ(mwctx->project.model.layers()[1]->type(), "yolo");

  // the head of the yolo layer takes the filters of it's classes
  EXPECT_EQ(rows()[0]->errors_label().get_text().raw(), "⚠ 1 wrong");

  rows()[0]->expander().set_expanded(true);

  auto* filters = rows()[0]->options()->rows().front().get();

  filters->entry()->set_text("18");

  EXPECT_EQ(rows()[0]->errors_label().get_text().raw(), "");

  click(rows()[0]->remove_button());
  iterate();

  ASSERT_EQ(rows().size(), 1U);
  EXPECT_EQ(rows()[0]->index_label().get_text().raw(), "0");
  EXPECT_EQ(mwctx->project.model.layers().size(), 1U);
  EXPECT_EQ(loader->layers_box()->get_first_child(),
            static_cast<Gtk::Widget*>(rows()[0]));
}

TEST_F(CTEST_NetEditorWindow, the_moved_layers_keep_their_references)
{
  show(project());

  LayerRow* route = rows()[2];

  // the route dropped onto the place of the maxpool
  mwctx->network->move_layer(2, 1);

  EXPECT_EQ(rows()[1], route);
  EXPECT_EQ(route->index_label().get_text().raw(), "1");
  EXPECT_EQ(route->summary_label().get_text().raw(), "layers=2,0");
  EXPECT_EQ(route->errors_label().get_text().raw(), "⚠ 1 wrong");
  EXPECT_EQ(loader->layers_box()->get_first_child()->get_next_sibling(),
            static_cast<Gtk::Widget*>(route));

  // the reference turned forward stays the absolute one of the same layer
  mwctx->network->move_layer(1, 2);

  EXPECT_EQ(route->summary_label().get_text().raw(), "layers=1,0");
  EXPECT_EQ(route->errors_label().get_text().raw(), "");
}

TEST_F(CTEST_NetEditorWindow, the_network_is_saved_into_the_opened_project)
{
  show(project());

  net_row("max_batches").entry()->set_text("8000");
  click(*loader->save_button());

  EXPECT_EQ(status(), "Saved into the " + file("net.dxxwjz3p"));
  EXPECT_FALSE(mwctx->project.modified());
  EXPECT_EQ(loader->file_label()->get_text().raw(), "net.dxxwjz3p");

  net_editor::NetworkProject saved;
  std::string message;

  ASSERT_TRUE(saved.open(file("net.dxxwjz3p"), message));
  EXPECT_EQ(*saved.model.net()->find("max_batches"), "8000");
}

TEST_F(CTEST_NetEditorWindow, the_wrong_network_is_not_saved)
{
  show(project());

  net_row("subdivisions").entry()->set_text("7");
  click(*loader->save_button());

  EXPECT_EQ(status(),
            "The network has 1 wrong options:\n[net] subdivisions: The 7 "
            "doesn't divide the batch=64");
  EXPECT_TRUE(loader->status_label()->has_css_class("error"));
  EXPECT_TRUE(mwctx->project.modified());
}

TEST_F(CTEST_NetEditorWindow, the_new_network_asks_for_the_file)
{
  show();
  click(*loader->save_button());

  auto* chooser = mwctx->file->save_chooser();

  ASSERT_NE(chooser, nullptr);
  EXPECT_TRUE(chooser->get_visible());
  EXPECT_EQ(chooser->get_current_name().raw(), "network.dxxwjz3p");

  // the canceled chooser saves nothing, the folder reads of it end first
  settle();
  chooser->response(Gtk::ResponseType::CANCEL);
  settle();

  EXPECT_FALSE(chooser->get_visible());
  EXPECT_EQ(mwctx->project.path(), "");

  // the chosen file is the project then
  ASSERT_TRUE(mwctx->file->save_into(file("chosen.dxxwjz3p")));
  EXPECT_EQ(loader->file_label()->get_text().raw(), "chosen.dxxwjz3p");
}

TEST_F(CTEST_NetEditorWindow, the_changed_network_asks_before_the_exit)
{
  show(project());

  net_row("max_batches").entry()->set_text("8000");
  loader->window()->close();
  iterate();

  auto* prompt = mwctx->file->prompt();

  ASSERT_NE(prompt, nullptr);
  EXPECT_TRUE(prompt->get_visible());
  EXPECT_TRUE(loader->window()->get_visible());

  // the cancel keeps the window and the changes
  prompt->response(Gtk::ResponseType::CANCEL);
  iterate();

  EXPECT_TRUE(loader->window()->get_visible());
  EXPECT_TRUE(mwctx->project.modified());

  // the save and exit saves into the project and closes the window
  loader->window()->close();
  iterate();
  mwctx->file->prompt()->response(ProjectEventsHandler::SAVE);
  iterate();

  EXPECT_FALSE(loader->window()->get_visible());
  EXPECT_FALSE(mwctx->project.modified());
}

TEST_F(CTEST_NetEditorWindow, the_discarded_changes_close_the_window)
{
  show(project());

  net_row("max_batches").entry()->set_text("8000");
  loader->window()->close();
  iterate();
  mwctx->file->prompt()->response(ProjectEventsHandler::DISCARD);
  iterate();

  EXPECT_FALSE(loader->window()->get_visible());

  net_editor::NetworkProject saved;
  std::string message;

  ASSERT_TRUE(saved.open(file("net.dxxwjz3p"), message));
  EXPECT_EQ(*saved.model.net()->find("max_batches"), "6000");
}

TEST_F(CTEST_NetEditorWindow, the_new_network_replaces_the_shown_one)
{
  show(project());

  click(*loader->new_button());

  EXPECT_TRUE(rows().empty());
  EXPECT_EQ(mwctx->project.path(), "");
  EXPECT_EQ(loader->classes_view()->get_buffer()->get_text().raw(), "");
}

TEST_F(CTEST_NetEditorWindow, the_classes_are_the_lines_of_names)
{
  show();

  loader->classes_view()->get_buffer()->set_text("cat\n\ndog\n");

  EXPECT_EQ(mwctx->project.model.classes,
            (std::vector<std::string>{"cat", "dog"}));
  EXPECT_TRUE(mwctx->project.modified());
}

TEST_F(CTEST_NetEditorWindow, an_os_stop_signal_closes_the_window)
{
  show();

  actx->stop.store(true);
  actx->stop_signal.store(2);

  ASSERT_TRUE(wait_for([&]() { return !loader->window()->get_visible(); }));
}
