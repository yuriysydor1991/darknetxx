#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <string>
#include <vector>

#include "src/app/ApplicationHelpPrinter.h"

using namespace app;
using namespace testing;

class UTEST_ApplicationHelpPrinter : public Test
{
 public:
  UTEST_ApplicationHelpPrinter()
      : app{std::make_shared<ApplicationHelpPrinter>()},
        appCtx{std::make_shared<ApplicationContext>(argc, argv)}
  {
  }

  int argc{0};
  char** argv{nullptr};

  std::shared_ptr<ApplicationHelpPrinter> app;
  std::shared_ptr<ApplicationContext> appCtx;
};

TEST_F(UTEST_ApplicationHelpPrinter, no_context_error)
{
  EXPECT_NE(app->run({}), 0);
}

TEST_F(UTEST_ApplicationHelpPrinter, normal_exit)
{
  EXPECT_CALL(*appCtx, push_error(_)).Times(0);

  EXPECT_EQ(app->run(appCtx), 0);

  EXPECT_TRUE(appCtx->errors.empty());

  EXPECT_FALSE(appCtx->print_help_and_exit);
  EXPECT_FALSE(appCtx->print_version_and_exit);
}

TEST_F(UTEST_ApplicationHelpPrinter, invalid_command_line_exit)
{
  appCtx->errors.emplace_back("Unknown parameter: --unknown");

  EXPECT_NE(app->run(appCtx), 0);
}

TEST_F(UTEST_ApplicationHelpPrinter, conversion_options_and_formats_are_listed)
{
  EXPECT_CALL(*appCtx, push_error(_)).Times(0);

  testing::internal::CaptureStdout();

  EXPECT_EQ(app->run(appCtx), 0);

  const std::string printed = testing::internal::GetCapturedStdout();

  EXPECT_NE(printed.find("darknetxx [OPTIONS]"), std::string::npos);

  for (const std::string& flag : std::vector<std::string>{
           "--convert-from", "--convert-src", "--convert-to", "--convert-dst",
           "--convert-cfg", "--convert-names", "weights", "dxxwjz1",
           "dxxwjz2"}) {
    EXPECT_NE(printed.find(flag), std::string::npos) << flag;
  }

  // the main binary lists the training and the detection as well
  EXPECT_NE(printed.find("--train-cfg"), std::string::npos);
  EXPECT_NE(printed.find("--detect-cfg"), std::string::npos);
  EXPECT_NE(printed.find("--save-format"), std::string::npos);
}
