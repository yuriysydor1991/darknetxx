#include <glibmm/main.h>
#include <gtest/gtest.h>
#include <gtk/gtk.h>
#include <gtkmm/init.h>

#include <chrono>
#include <filesystem>
#include <fstream>
#include <functional>
#include <memory>
#include <string>
#include <thread>

#include "LibraryFacade.h"
#include "src/app/ApplicationContext.h"
#include "src/gui/gtkmm4/recognition-window/RecognitionWindowContext.h"
#include "src/tests/component/bias-networks/BiasNetworks.h"

using darknetxxGtkmm4::RecognitionWindowContext;

namespace fs = std::filesystem;

class CTEST_RecognitionWindow : public testing::Test
{
 public:
  void SetUp() override
  {
    using namespace bias_networks;

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

    write(file("detector.cfg"), file("detector.weights"), DETECTOR_CFG,
          DETECTOR_BIASES, DETECTOR_WEIGHTS);
    write(file("classifier.cfg"), file("classifier.weights"), CLASSIFIER_CFG,
          CLASSIFIER_BIASES, CLASSIFIER_WEIGHTS);
    write_image(file("image.ppm"));
    std::ofstream{file("abc.names")} << "a\nb\nc\n";

    libctx->stop_with(actx->stop);
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

  /// @brief Shows the window of the files of the command line.
  void show(const std::string& network = "", const std::string& image = "")
  {
    libctx->detect_cfg = network.empty() ? "" : file(network + ".cfg");
    libctx->latest_weights = network.empty() ? "" : file(network + ".weights");
    libctx->names = network.empty() ? "" : file("abc.names");
    libctx->image = image.empty() ? "" : file(image);

    mwctx = std::make_shared<RecognitionWindowContext>(actx, libctx);
    loader = &mwctx->wloader;

    ASSERT_TRUE(mwctx->load());

    loader->window()->present();
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

  /// @brief Waits for the shown recognition, the Recognize button is
  /// sensitive again.
  bool recognized()
  {
    return wait_for(
        [&]() { return loader->recognize_button()->get_sensitive(); });
  }

  void enter(Gtk::Entry* entry, const std::string& text)
  {
    entry->set_text(text);
    g_signal_emit_by_name(entry->gobj(), "activate");
  }

  std::string status() { return loader->status_label()->get_text().raw(); }
  std::string objects() { return loader->objects_label()->get_text().raw(); }

  fs::path dir{fs::path{CTEST_RecognitionWindow_DATA_DIR} /
               testing::UnitTest::GetInstance()->current_test_info()->name()};
  int argc{0};
  char** argv{nullptr};
  std::shared_ptr<app::ApplicationContext> actx{
      std::make_shared<app::ApplicationContext>(argc, argv)};
  darknetxx011::LibraryContextPtr libctx{
      darknetxx011::LibraryFacade::create_library_context()};
  std::shared_ptr<RecognitionWindowContext> mwctx;
  darknetxxGtkmm4::window::RecognitionWindowLoader* loader{nullptr};
};

TEST_F(CTEST_RecognitionWindow, the_command_line_files_are_recognized_at_once)
{
  show("detector", "image.ppm");

  EXPECT_EQ(loader->weights_entry()->get_text().raw(),
            file("detector.weights"));
  EXPECT_FALSE(loader->recognize_button()->get_sensitive());
  ASSERT_TRUE(recognized());
  EXPECT_EQ(status().rfind("1 object recognized in ", 0), 0U) << status();
  EXPECT_EQ(objects(), "b: 100.0% (left_x: 0 top_y: 0 width: 31 height: 23)\n");

  // enlarged for the labels
  const auto picture = loader->image_picture()->get_paintable();

  ASSERT_NE(picture, nullptr);
  EXPECT_EQ(picture->get_intrinsic_width(), 32 * 15);
  EXPECT_EQ(picture->get_intrinsic_height(), 24 * 15);
}

TEST_F(CTEST_RecognitionWindow, the_missing_files_are_told)
{
  show();

  // clicked once the pressed button is shown
  loader->recognize_button()->activate();

  EXPECT_TRUE(wait_for(
      [&]() { return status() == "Choose the weights file of the network"; }));

  enter(loader->weights_entry(), file("detector.weights"));

  EXPECT_EQ(status(), "Choose the image to recognize");
  EXPECT_TRUE(loader->recognize_button()->get_sensitive());
}

TEST_F(CTEST_RecognitionWindow, the_changed_threshold_recognizes_again)
{
  show();

  loader->thresh_spin()->set_value(.0);
  loader->cfg_entry()->set_text(file("classifier.cfg"));
  loader->names_entry()->set_text(file("abc.names"));
  loader->image_entry()->set_text(file("image.ppm"));
  enter(loader->weights_entry(), file("classifier.weights"));

  ASSERT_TRUE(recognized());
  EXPECT_EQ(objects(), "c: 98.7%\na: 0.7%\nb: 0.7%\n");

  loader->thresh_spin()->set_value(.5);

  ASSERT_TRUE(recognized());
  EXPECT_EQ(objects(), "c: 98.7%\n");
}

TEST_F(CTEST_RecognitionWindow, the_failure_is_told)
{
  show("detector", "abc.names");

  ASSERT_TRUE(recognized());
  EXPECT_EQ(status(), "Couldn't read the image " + file("abc.names"));
  EXPECT_EQ(objects(), "");
  EXPECT_EQ(loader->image_picture()->get_paintable(), nullptr);
}

TEST_F(CTEST_RecognitionWindow,
       the_entries_changed_meanwhile_are_recognized_next)
{
  show("detector", "image.ppm");

  // the recognition of the command line files is not shown yet
  enter(loader->image_entry(), file("abc.names"));

  ASSERT_TRUE(recognized());
  EXPECT_EQ(status(), "Couldn't read the image " + file("abc.names"));
}

TEST_F(CTEST_RecognitionWindow, the_dxxwjz2p_project_alone_is_the_network)
{
  const darknetxx011::LibraryContextPtr conversion =
      darknetxx011::LibraryFacade::create_library_context();

  conversion->convert_from = "weights";
  conversion->convert_src = file("detector.weights");
  conversion->convert_to = "dxxwjz2p";
  conversion->convert_dst = file("detector.dxxwjz2p");
  conversion->convert_cfg = file("detector.cfg");
  conversion->convert_names = file("abc.names");

  ASSERT_TRUE(darknetxx011::LibraryFacade::create_library(conversion)
                  ->libcall(conversion));

  show();

  loader->image_entry()->set_text(file("image.ppm"));
  enter(loader->weights_entry(), file("detector.dxxwjz2p"));

  ASSERT_TRUE(recognized());
  EXPECT_EQ(objects(), "b: 100.0% (left_x: 0 top_y: 0 width: 31 height: 23)\n");
}

TEST_F(CTEST_RecognitionWindow, an_os_stop_signal_closes_the_window)
{
  show();

  actx->stop.store(true);
  actx->stop_signal.store(2);

  ASSERT_TRUE(wait_for([&]() { return !loader->window()->get_visible(); }));
}
