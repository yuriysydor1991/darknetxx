#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>

#include "LibraryFacade.h"
#include "src/app/Application.h"
#include "src/app/ApplicationFactory.h"
#include "src/app/ApplicationHelpPrinter.h"
#include "src/app/ApplicationVersionPrinter.h"
#include "src/app/NetworkCfgPrinter.h"
#include "src/app/signals-handlers/SignalsHandler.h"

using namespace app;
using namespace testing;

class UTEST_ConverterOnlyApplicationFactory : public Test
{
 public:
  UTEST_ConverterOnlyApplicationFactory()
      : factory{std::make_shared<ApplicationFactory>()}
  {
  }

  ~UTEST_ConverterOnlyApplicationFactory()
  {
    CommandLineParser::onMockCreate = nullptr;
    ApplicationHelpPrinter::onMockCreate = nullptr;
    Application::onMockCreate = nullptr;
    SignalsHandler::onMockCreate = nullptr;
  }

  std::shared_ptr<ApplicationFactory> factory;

  int customArgc{0};
  char** customArgv{nullptr};
};

TEST_F(UTEST_ConverterOnlyApplicationFactory,
       default_application_is_the_help_printer)
{
  // there is no training nor detection to start
  EXPECT_NE(std::dynamic_pointer_cast<ApplicationHelpPrinter>(
                factory->create_default_application()),
            nullptr);
}

TEST_F(UTEST_ConverterOnlyApplicationFactory,
       empty_command_line_is_the_failed_help)
{
  auto ctx = std::make_shared<ApplicationContext>(customArgc, customArgv);

  // the help printer reports the failure of the command line
  EXPECT_CALL(*ctx, push_error(HasSubstr("No network weights file "
                                         "conversion was requested")))
      .Times(1);

  EXPECT_NE(std::dynamic_pointer_cast<ApplicationHelpPrinter>(
                factory->create_application(ctx)),
            nullptr);
}

TEST_F(UTEST_ConverterOnlyApplicationFactory,
       conversion_creates_the_converter_application)
{
  auto ctx = std::make_shared<ApplicationContext>(customArgc, customArgv);

  ctx->convert_from = "dxxwjz1";
  ctx->convert_src = "a.dxxwjz1";
  ctx->convert_to = "weights";
  ctx->convert_dst = "a.weights";

  EXPECT_CALL(*ctx, push_error(_)).Times(0);

  EXPECT_NE(
      std::dynamic_pointer_cast<Application>(factory->create_application(ctx)),
      nullptr);
}

TEST_F(UTEST_ConverterOnlyApplicationFactory, cfg_dump_creates_the_cfg_printer)
{
  auto ctx = std::make_shared<ApplicationContext>(customArgc, customArgv);

  ctx->dump_cfg = "net.dxxwjz3";

  EXPECT_CALL(*ctx, push_error(_)).Times(0);

  EXPECT_NE(std::dynamic_pointer_cast<NetworkCfgPrinter>(
                factory->create_application(ctx)),
            nullptr);
}

TEST_F(UTEST_ConverterOnlyApplicationFactory, chart_is_no_work_of_the_converter)
{
  auto ctx = std::make_shared<ApplicationContext>(customArgc, customArgv);

  ctx->latest_weights = "net.dxxwjz2";
  ctx->draw_chart = "chart.png";

  EXPECT_CALL(*ctx, push_error(_)).Times(1);

  EXPECT_NE(std::dynamic_pointer_cast<ApplicationHelpPrinter>(
                factory->create_application(ctx)),
            nullptr);
}

TEST_F(UTEST_ConverterOnlyApplicationFactory, help_and_version_are_printed)
{
  auto ctx = std::make_shared<ApplicationContext>(customArgc, customArgv);

  ctx->print_version_and_exit = true;

  EXPECT_NE(std::dynamic_pointer_cast<ApplicationVersionPrinter>(
                factory->create_application(ctx)),
            nullptr);

  ctx->print_help_and_exit = true;

  EXPECT_NE(std::dynamic_pointer_cast<ApplicationHelpPrinter>(
                factory->create_application(ctx)),
            nullptr);
}

TEST_F(UTEST_ConverterOnlyApplicationFactory, run_of_the_conversion)
{
  MockFunction<void(CommandLineParser & instance)> onMockCreateParserEnsurer;
  MockFunction<void(Application & instance)> onMockCreateAppEnsurer;
  MockFunction<void(SignalsHandler & instance)> onMockCreateSignalsEnsurer;

  EXPECT_CALL(onMockCreateParserEnsurer, Call(_))
      .Times(1)
      .WillOnce(Invoke([&](CommandLineParser& instance) {
        EXPECT_CALL(instance, parse_args(_))
            .Times(1)
            .WillOnce(Invoke([&](std::shared_ptr<ApplicationContext> ctx) {
              ctx->convert_from = "weights";
              ctx->convert_src = "a.weights";
              ctx->convert_to = "dxxwjz1";
              ctx->convert_dst = "a.dxxwjz1";
              ctx->convert_cfg = "a.cfg";
              return true;
            }));
      }));

  EXPECT_CALL(onMockCreateAppEnsurer, Call(_))
      .Times(1)
      .WillOnce(Invoke([&](Application& instance) {
        EXPECT_CALL(instance, run(_)).Times(1).WillOnce(Return(0));
      }));

  EXPECT_CALL(onMockCreateSignalsEnsurer, Call(_))
      .Times(1)
      .WillOnce(Invoke([](SignalsHandler& instance) {
        EXPECT_CALL(instance, install(_)).Times(1).WillOnce(Return(true));
      }));

  CommandLineParser::onMockCreate = onMockCreateParserEnsurer.AsStdFunction();
  Application::onMockCreate = onMockCreateAppEnsurer.AsStdFunction();
  SignalsHandler::onMockCreate = onMockCreateSignalsEnsurer.AsStdFunction();

  EXPECT_EQ(factory->run(customArgc, customArgv), 0);
}
