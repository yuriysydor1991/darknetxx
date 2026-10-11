#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>
#include <string>

#include "LibraryFacade.h"
#include "src/app/applications/NetworkCfgPrinter.h"

using namespace app;
using namespace testing;
using darknetxx011::LibraryFacade;

class UTEST_NetworkCfgPrinter : public Test
{
 public:
  ~UTEST_NetworkCfgPrinter() override { LibraryFacade::reset(); }

  int argc{0};
  char** argv{nullptr};
  std::shared_ptr<NetworkCfgPrinter> app{std::make_shared<NetworkCfgPrinter>()};
  std::shared_ptr<ApplicationContext> appCtx{
      std::make_shared<ApplicationContext>(argc, argv)};
};

TEST_F(UTEST_NetworkCfgPrinter, no_context_error)
{
  EXPECT_NE(app->run({}), 0);
}

TEST_F(UTEST_NetworkCfgPrinter, the_cfg_alone_is_printed)
{
  const std::string cfg{"[net]\nbatch=64\n\n[maxpool]\nsize=2\n"};

  appCtx->dump_cfg = "net.dxxwjz3";

  LibraryFacade::on_network_cfg = [&cfg](const std::string& weights) {
    EXPECT_EQ(weights, "net.dxxwjz3");
    return cfg;
  };

  testing::internal::CaptureStdout();

  EXPECT_EQ(app->run(appCtx), 0);
  EXPECT_EQ(testing::internal::GetCapturedStdout(), cfg);
}

TEST_F(UTEST_NetworkCfgPrinter, the_file_of_no_cfg_is_an_error)
{
  appCtx->dump_cfg = "net.dxxwjz1";

  EXPECT_EQ(app->run(appCtx), IApplication::INVALID);
}
