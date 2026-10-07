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
#include "src/gui/gtkmm4/MainWindowContext.h"

using darknetxxGtkmm4::MainWindowContext;

namespace fs = std::filesystem;

class CTEST_GtkmmWindow : public testing::Test
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

    fs::remove_all(folder);
    fs::copy(CTEST_GtkmmWindow_YOLO4_DIR, folder, fs::copy_options::recursive);

    // the way the GUI application trains
    libctx->dont_show = true;
    libctx->dont_save_charts_every_iter = false;
    libctx->stop_with(actx->stop);

    mwctx = MainWindowContext::build_context(actx, libctx);
    loader = mwctx->wloader.get();

    ASSERT_TRUE(loader->load());
    ASSERT_TRUE(mwctx->weHandler->init(mwctx));

    loader->window()->present();
  }

  void TearDown() override
  {
    if (mwctx != nullptr) {
      mwctx->clear();
    }

    fs::current_path(workingDir);
    fs::remove_all(folder);
  }

  /// @brief Runs the main loop till the condition holds or the time is out.
  static bool wait_for(const std::function<bool()>& condition)
  {
    const auto end = std::chrono::steady_clock::now() + std::chrono::minutes{5};

    while (!condition()) {
      if (std::chrono::steady_clock::now() > end) {
        return false;
      }

      Glib::MainContext::get_default()->iteration(false);
      std::this_thread::sleep_for(std::chrono::milliseconds{5});
    }

    return true;
  }

  void open(const fs::path& path)
  {
    loader->folder_entry()->set_text(path.string());
    g_signal_emit_by_name(loader->folder_entry()->gobj(), "activate");
  }

  bool status_has(const std::string& text)
  {
    return loader->status_label()->get_text().raw().find(text) !=
           std::string::npos;
  }

  fs::path folder{
      fs::path{CTEST_GtkmmWindow_DATA_DIR} /
      testing::UnitTest::GetInstance()->current_test_info()->name()};
  fs::path workingDir{fs::current_path()};
  int argc{0};
  char** argv{nullptr};
  std::shared_ptr<app::ApplicationContext> actx{
      std::make_shared<app::ApplicationContext>(argc, argv)};
  darknetxx011::LibraryContextPtr libctx{
      darknetxx011::LibraryFacade::create_library_context()};
  std::shared_ptr<MainWindowContext> mwctx;
  darknetxxGtkmm4::window::WindowLoader* loader{nullptr};
};

TEST_F(CTEST_GtkmmWindow, the_folder_entry_starts_with_the_working_directory)
{
  EXPECT_EQ(loader->folder_entry()->get_text().raw(),
            fs::current_path().string());
  EXPECT_FALSE(loader->training_box()->get_visible());
}

TEST_F(CTEST_GtkmmWindow, a_folder_without_a_cfg_is_reported)
{
  fs::remove_all(folder / "cfg");

  open(folder);

  EXPECT_TRUE(status_has("No cfg/*.cfg network file found"));
  EXPECT_FALSE(loader->training_box()->get_visible());
  EXPECT_FALSE(mwctx->trainer->running());
}

TEST_F(CTEST_GtkmmWindow, the_training_shows_it_s_progress_stops_and_resumes)
{
  open(folder);

  ASSERT_TRUE(mwctx->trainer->running());
  EXPECT_TRUE(status_has("from scratch"));
  EXPECT_TRUE(loader->training_box()->get_visible());
  EXPECT_FALSE(loader->folder_entry()->get_sensitive());
  EXPECT_FALSE(loader->folder_button()->get_sensitive());
  EXPECT_EQ(loader->train_button()->get_label(), "_Stop");

  ASSERT_TRUE(wait_for([&]() {
    return loader->progress_bar()->get_fraction() > 0.0 &&
           loader->chart_picture()->get_paintable() != nullptr;
  }));

  EXPECT_EQ(loader->loss_label()->get_text().raw().rfind("Loss ", 0), 0U);
  EXPECT_NE(loader->details_label()->get_text().raw().find("avg loss"),
            std::string::npos);

  loader->train_button()->activate();

  ASSERT_TRUE(wait_for(
      [&]() { return loader->train_button()->get_label() == "_Resume"; }));

  EXPECT_FALSE(mwctx->trainer->running());
  EXPECT_TRUE(status_has("stopped"));
  EXPECT_TRUE(loader->folder_entry()->get_sensitive());
  EXPECT_TRUE(loader->folder_button()->get_sensitive());

  loader->train_button()->activate();

  ASSERT_TRUE(wait_for([&]() { return mwctx->trainer->running(); }));
  EXPECT_TRUE(status_has("Resuming the training from " +
                         (folder / "backup" / "digits_last.dxxwjz2").string()));
}

TEST_F(CTEST_GtkmmWindow, the_recorded_chart_shows_up_once_the_folder_opens)
{
  open(folder);

  ASSERT_TRUE(
      wait_for([&]() { return loader->progress_bar()->get_fraction() > 0.0; }));

  loader->train_button()->activate();

  ASSERT_TRUE(wait_for(
      [&]() { return loader->train_button()->get_label() == "_Resume"; }));

  // no chart image, the record of the saved weights alone
  for (const auto& entry : fs::directory_iterator{folder}) {
    if (entry.path().extension() == ".png") {
      fs::remove(entry.path());
    }
  }

  loader->chart_picture()->set_paintable({});

  open(folder);

  // at once, before any iteration of the resumed training
  const auto chart = loader->chart_picture()->get_paintable();

  ASSERT_NE(chart, nullptr);
  EXPECT_EQ(chart->get_intrinsic_width(), 1000);
  EXPECT_EQ(chart->get_intrinsic_height(), 1000);
}

TEST_F(CTEST_GtkmmWindow, the_close_waits_for_the_stopped_training)
{
  open(folder);

  // a training stopped before it's first iteration has no weights to save
  ASSERT_TRUE(
      wait_for([&]() { return loader->progress_bar()->get_fraction() > 0.0; }));

  loader->window()->close();

  EXPECT_TRUE(loader->window()->get_visible());
  EXPECT_TRUE(status_has("Stopping the training to close the window"));

  ASSERT_TRUE(wait_for([&]() { return !loader->window()->get_visible(); }));

  EXPECT_FALSE(mwctx->trainer->running());
  EXPECT_TRUE(fs::exists(folder / "backup" / "digits_last.dxxwjz2"));
}

TEST_F(CTEST_GtkmmWindow, an_os_stop_signal_closes_the_window)
{
  actx->stop.store(true);
  actx->stop_signal.store(2);

  ASSERT_TRUE(wait_for([&]() { return !loader->window()->get_visible(); }));
}
