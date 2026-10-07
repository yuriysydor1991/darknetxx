#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>

#include "LibraryFacade.h"
#include "src/app/Application.h"
#include "src/app/ApplicationFactory.h"
#include "src/app/ApplicationHelpPrinter.h"
#include "src/app/GuiApplication.h"
#include "src/app/signals-handlers/SignalsHandler.h"

using namespace app;
using namespace testing;

class UTEST_GuiApplicationFactory : public Test
{
 public:
  ~UTEST_GuiApplicationFactory() override
  {
    CommandLineParser::onMockCreate = nullptr;
    GuiApplication::onMockCreate = nullptr;
    SignalsHandler::onMockCreate = nullptr;
  }

  std::shared_ptr<ApplicationFactory> factory{
      std::make_shared<ApplicationFactory>()};
  int customArgc{0};
  char** customArgv{nullptr};
};

TEST_F(UTEST_GuiApplicationFactory, default_application_is_the_gui_one)
{
  EXPECT_NE(std::dynamic_pointer_cast<GuiApplication>(
                factory->create_default_application()),
            nullptr);
}

TEST_F(UTEST_GuiApplicationFactory, help_and_conversion_stay_available)
{
  auto ctx = std::make_shared<ApplicationContext>(customArgc, customArgv);

  ctx->convert_from = "dxxwjz1";
  ctx->convert_src = "a.dxxwjz1";
  ctx->convert_to = "weights";
  ctx->convert_dst = "a.weights";

  EXPECT_NE(
      std::dynamic_pointer_cast<Application>(factory->create_application(ctx)),
      nullptr);

  ctx->print_help_and_exit = true;

  EXPECT_NE(std::dynamic_pointer_cast<ApplicationHelpPrinter>(
                factory->create_application(ctx)),
            nullptr);
}

TEST_F(UTEST_GuiApplicationFactory, the_chart_is_drawn_without_a_window)
{
  auto ctx = std::make_shared<ApplicationContext>(customArgc, customArgv);

  ctx->latest_weights = "net.dxxwjz2";
  ctx->draw_chart = "chart.png";

  EXPECT_NE(
      std::dynamic_pointer_cast<Application>(factory->create_application(ctx)),
      nullptr);
}

TEST_F(UTEST_GuiApplicationFactory, run_of_the_gui)
{
  MockFunction<void(CommandLineParser & instance)> onMockCreateParserEnsurer;
  MockFunction<void(GuiApplication & instance)> onMockCreateAppEnsurer;
  MockFunction<void(SignalsHandler & instance)> onMockCreateSignalsEnsurer;

  EXPECT_CALL(onMockCreateParserEnsurer, Call(_))
      .Times(1)
      .WillOnce(Invoke([](CommandLineParser& instance) {
        EXPECT_CALL(instance, parse_args(_)).Times(1).WillOnce(Return(true));
      }));

  EXPECT_CALL(onMockCreateAppEnsurer, Call(_))
      .Times(1)
      .WillOnce(Invoke([](GuiApplication& instance) {
        EXPECT_CALL(instance, run(_)).Times(1).WillOnce(Return(0));
      }));

  EXPECT_CALL(onMockCreateSignalsEnsurer, Call(_))
      .Times(1)
      .WillOnce(Invoke([](SignalsHandler& instance) {
        EXPECT_CALL(instance, install(_)).Times(1).WillOnce(Return(true));
      }));

  CommandLineParser::onMockCreate = onMockCreateParserEnsurer.AsStdFunction();
  GuiApplication::onMockCreate = onMockCreateAppEnsurer.AsStdFunction();
  SignalsHandler::onMockCreate = onMockCreateSignalsEnsurer.AsStdFunction();

  EXPECT_EQ(factory->run(customArgc, customArgv), 0);
}
