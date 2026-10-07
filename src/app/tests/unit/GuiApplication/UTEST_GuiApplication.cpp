#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <csignal>

#include "LibraryFacade.h"
#include "src/app/GuiApplication.h"
#include "src/gui/gtkmm4/GtkmmIniter.h"

using namespace app;
using namespace testing;
using darknetxxGtkmm4::GtkmmIniter;

class UTEST_GuiApplication : public Test
{
 public:
  UTEST_GuiApplication()
  {
    // the window runs the training, the application itself does not
    darknetxx011::LibraryFacade::on_create_library =
        [](darknetxx011::LibraryContextPtr) {
          ADD_FAILURE() << "The GUI application performed the work itself";
          return nullptr;
        };
  }

  ~UTEST_GuiApplication() override
  {
    darknetxx011::LibraryFacade::reset();
    GtkmmIniter::onMockCreate = nullptr;
  }

  /// @brief The window run returns the given result, after the given work.
  void expect_window(
      const bool shown, std::function<void(AppCtxPtr)> work = [](AppCtxPtr) {})
  {
    GtkmmIniter::onMockCreate = [=](GtkmmIniter& initer) {
      EXPECT_CALL(initer, run(_, _))
          .WillOnce([=](AppCtxPtr ctx, darknetxx011::LibraryContextPtr libctx) {
            work(ctx);
            mlibctx = libctx;
            return shown;
          });
      EXPECT_CALL(initer, deinit()).Times(1);
    };
  }

  int argc{0};
  char** argv{nullptr};
  inline static darknetxx011::LibraryContextPtr mlibctx;
  std::shared_ptr<GuiApplication> app{std::make_shared<GuiApplication>()};
  std::shared_ptr<ApplicationContext> appCtx{
      std::make_shared<ApplicationContext>(argc, argv)};
};

TEST_F(UTEST_GuiApplication, the_closed_window_is_the_success)
{
  expect_window(true);

  EXPECT_EQ(app->run(appCtx), 0);

  // the window shows the chart the training saves on every iteration
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
