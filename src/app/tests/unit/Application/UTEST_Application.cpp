#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <csignal>
#include <filesystem>
#include <fstream>
#include <iterator>
#include <memory>
#include <string>
#include <vector>

#include "ILibMock.h"
#include "LibraryFacade.h"
#include "src/app/Application.h"

using namespace app;
using namespace testing;
using darknetxx011::ILibMock;
using darknetxx011::LibraryContextPtr;
using darknetxx011::LibraryFacade;

class UTEST_Application : public Test
{
 public:
  ~UTEST_Application() override
  {
    LibraryFacade::reset();
    converters::ApplicationContext2LibraryContext::onMockCreate = nullptr;
  }

  /// @brief The library work the context asks for ends with the given
  /// result, after the given work.
  void expect_library_work(
      const bool performed, std::function<void()> work = []() {})
  {
    LibraryFacade::on_create_library = [=](LibraryContextPtr) {
      auto lib = std::make_shared<ILibMock>();

      EXPECT_CALL(*lib, libcall(_)).WillOnce([=](LibraryContextPtr) {
        work();
        return performed;
      });

      return lib;
    };
  }

  int argc{0};
  char** argv{nullptr};
  std::shared_ptr<Application> app{std::make_shared<Application>()};
  std::shared_ptr<ApplicationContext> appCtx{
      std::make_shared<ApplicationContext>(argc, argv)};
};

TEST_F(UTEST_Application, no_context_error) { EXPECT_NE(app->run({}), 0); }

TEST_F(UTEST_Application, the_performed_library_work_is_the_success)
{
  expect_library_work(true);

  EXPECT_CALL(*appCtx, push_error(_)).Times(0);

  EXPECT_EQ(app->run(appCtx), 0);
}

TEST_F(UTEST_Application, the_application_context_is_converted)
{
  MockFunction<void(converters::ApplicationContext2LibraryContext&)> created;

  EXPECT_CALL(created, Call(_))
      .WillOnce([this](converters::ApplicationContext2LibraryContext& ctx) {
        EXPECT_CALL(ctx, convert(Eq(appCtx), NotNull())).WillOnce(Return(true));
      });

  converters::ApplicationContext2LibraryContext::onMockCreate =
      created.AsStdFunction();

  expect_library_work(true);

  EXPECT_EQ(app->run(appCtx), 0);
}

TEST_F(UTEST_Application, no_library_work_is_an_error)
{
  EXPECT_EQ(app->run(appCtx), IApplication::INVALID);
}

TEST_F(UTEST_Application, the_failed_work_is_an_error)
{
  expect_library_work(false);

  EXPECT_EQ(app->run(appCtx), IApplication::INVALID);
}

TEST_F(UTEST_Application, the_stopped_work_exits_with_the_signal_status)
{
  expect_library_work(true, [this]() {
    appCtx->stop_signal.store(SIGINT);
    appCtx->stop.store(true);
  });

  EXPECT_EQ(app->run(appCtx), 128 + SIGINT);
}

TEST_F(UTEST_Application, the_stopped_failed_work_exits_with_the_signal_status)
{
  expect_library_work(false, [this]() {
    appCtx->stop_signal.store(SIGTERM);
    appCtx->stop.store(true);
  });

  EXPECT_EQ(app->run(appCtx), 128 + SIGTERM);
}

TEST_F(UTEST_Application, the_chart_is_drawn_without_the_library_work)
{
  const std::string png =
      (std::filesystem::temp_directory_path() / "UTEST_Application.png")
          .string();

  appCtx->latest_weights = "net.dxxwjz2";
  appCtx->draw_chart = png;

  LibraryFacade::on_training_chart = [](const std::string& weights) {
    EXPECT_EQ(weights, "net.dxxwjz2");
    return std::vector<unsigned char>{0x89U, 'P', 'N', 'G'};
  };

  EXPECT_EQ(app->run(appCtx), 0);

  std::ifstream file{png, std::ios::binary};

  EXPECT_EQ(std::string(std::istreambuf_iterator<char>{file}, {}), "\x89PNG");

  std::filesystem::remove(png);

  // the weights file of no training record has no chart
  LibraryFacade::on_training_chart = [](const std::string&) {
    return std::vector<unsigned char>{};
  };

  EXPECT_NE(app->run(appCtx), 0);
  EXPECT_FALSE(std::filesystem::exists(png));
}
