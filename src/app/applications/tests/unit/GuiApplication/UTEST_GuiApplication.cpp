#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <csignal>
#include <functional>

#include "LibraryFacade.h"
#include "src/app/applications/NetEditorGuiApplication.h"
#include "src/app/applications/RecognitionGuiApplication.h"
#include "src/app/applications/TrainGuiApplication.h"
#include "src/gui/gtkmm4/net-editor-window/NetEditorGtkmmIniter.h"
#include "src/gui/gtkmm4/recognition-window/RecognitionGtkmmIniter.h"
#include "src/gui/gtkmm4/train-window/TrainGtkmmIniter.h"

using namespace app;
using namespace testing;

namespace
{

class MockGUIIniter : public gui::IGUIIniter
{
 public:
  MOCK_METHOD(bool, run,
              (AppCtxPtr ctx, darknetxx011::LibraryContextPtr libctx),
              (override));
  MOCK_METHOD(void, deinit, (), (override));
};

/// @brief The GUI binary application of the mock GUI.
class MockGuiApplication : public GuiApplication
{
 public:
  std::shared_ptr<MockGUIIniter> initer{std::make_shared<MockGUIIniter>()};

 protected:
  std::shared_ptr<gui::IGUIIniter> create_gui_initer() override
  {
    return initer;
  }
};

/// @brief The GUI binary application of it's GUI told.
template <class Application>
class OpenGuiApplication : public Application
{
 public:
  using Application::create_gui_initer;
};

}  // namespace

class UTEST_GuiApplication : public Test
{
 public:
  UTEST_GuiApplication()
  {
    // the window runs the work, the application itself does not
    darknetxx011::LibraryFacade::on_create_library =
        [](darknetxx011::LibraryContextPtr) {
          ADD_FAILURE() << "The GUI application performed the work itself";
          return nullptr;
        };
  }

  ~UTEST_GuiApplication() override { darknetxx011::LibraryFacade::reset(); }

  /// @brief The window run returns the given result, after the given work.
  void expect_window(
      const bool shown, std::function<void(AppCtxPtr)> work = [](AppCtxPtr) {})
  {
    EXPECT_CALL(*app->initer, run(_, _))
        .WillOnce([this, shown, work](AppCtxPtr ctx,
                                      darknetxx011::LibraryContextPtr libctx) {
          work(ctx);
          mlibctx = libctx;
          return shown;
        });
    EXPECT_CALL(*app->initer, deinit()).Times(1);
  }

  int argc{0};
  char** argv{nullptr};
  darknetxx011::LibraryContextPtr mlibctx;
  std::shared_ptr<MockGuiApplication> app{
      std::make_shared<MockGuiApplication>()};
  std::shared_ptr<ApplicationContext> appCtx{
      std::make_shared<ApplicationContext>(argc, argv)};
};

TEST_F(UTEST_GuiApplication, the_closed_window_is_the_success)
{
  expect_window(true);

  EXPECT_EQ(app->run(appCtx), 0);

  // the training window shows the chart the training saves on every iteration
  ASSERT_NE(mlibctx, nullptr);
  EXPECT_TRUE(mlibctx->dont_show);
  EXPECT_FALSE(mlibctx->dont_save_charts_every_iter);
}

TEST_F(UTEST_GuiApplication, the_window_error_is_an_error)
{
  expect_window(false);

  EXPECT_EQ(app->run(appCtx), IApplication::INVALID);
}

TEST_F(UTEST_GuiApplication, the_os_stop_signal_exits_with_the_signal_status)
{
  expect_window(true, [](AppCtxPtr ctx) {
    ctx->stop_signal.store(SIGTERM);
    ctx->stop.store(true);
  });

  EXPECT_EQ(app->run(appCtx), 128 + SIGTERM);
}

TEST_F(UTEST_GuiApplication, the_stop_button_is_not_the_os_stop_signal)
{
  expect_window(true, [](AppCtxPtr ctx) { ctx->stop.store(true); });

  EXPECT_EQ(app->run(appCtx), 0);
}

TEST_F(UTEST_GuiApplication, every_gui_binary_shows_it_s_own_window)
{
  EXPECT_NE(std::dynamic_pointer_cast<darknetxxGtkmm4::TrainGtkmmIniter>(
                OpenGuiApplication<TrainGuiApplication>{}.create_gui_initer()),
            nullptr);
  EXPECT_NE(
      std::dynamic_pointer_cast<darknetxxGtkmm4::RecognitionGtkmmIniter>(
          OpenGuiApplication<RecognitionGuiApplication>{}.create_gui_initer()),
      nullptr);
  EXPECT_NE(
      std::dynamic_pointer_cast<darknetxxGtkmm4::NetEditorGtkmmIniter>(
          OpenGuiApplication<NetEditorGuiApplication>{}.create_gui_initer()),
      nullptr);
}
