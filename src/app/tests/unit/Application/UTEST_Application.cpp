#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <csignal>

#include "src/app/Application.h"
#include "src/darknet-adaptor/DarknetController.h"

using namespace app;
using namespace testing;

class UTEST_Application : public Test
{
 public:
  UTEST_Application()
      : app{std::make_shared<Application>()},
        appCtx{std::make_shared<ApplicationContext>(argc, argv)}
  {
  }

  int argc{0};
  char** argv{nullptr};

  ~UTEST_Application() override
  {
    darknet_adaptor::DarknetController::on_perform = nullptr;
  }

  std::shared_ptr<Application> app;
  std::shared_ptr<ApplicationContext> appCtx;
};

TEST_F(UTEST_Application, no_context_error) { EXPECT_NE(app->run({}), 0); }

TEST_F(UTEST_Application, normal_exit)
{
  EXPECT_CALL(*appCtx, push_error(_)).Times(0);

  EXPECT_EQ(app->run(appCtx), 0);

  EXPECT_TRUE(appCtx->errors.empty());

  EXPECT_FALSE(appCtx->print_help_and_exit);
  EXPECT_FALSE(appCtx->print_version_and_exit);
}

TEST_F(UTEST_Application, the_stopped_work_exits_with_the_signal_status)
{
  darknet_adaptor::DarknetController::on_perform =
      [](app::AppCtxPtr ctx) -> bool {
    ctx->stop_signal.store(SIGINT);
    ctx->stop.store(true);
    return true;
  };

  EXPECT_EQ(app->run(appCtx), 128 + SIGINT);
}

TEST_F(UTEST_Application, the_stopped_failed_work_exits_with_the_signal_status)
{
  darknet_adaptor::DarknetController::on_perform =
      [](app::AppCtxPtr ctx) -> bool {
    ctx->stop_signal.store(SIGTERM);
    ctx->stop.store(true);
    return false;
  };

  EXPECT_EQ(app->run(appCtx), 128 + SIGTERM);
}

TEST_F(UTEST_Application, the_failed_work_is_an_error)
{
  darknet_adaptor::DarknetController::on_perform = [](app::AppCtxPtr) -> bool {
    return false;
  };

  EXPECT_EQ(app->run(appCtx), IApplication::INVALID);
}
