#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <chrono>
#include <csignal>
#include <cstdlib>
#include <memory>
#include <thread>

#include "src/app/signals-handlers/handlers/posix/StopSignalsHandler.h"

using namespace app;
using namespace testing;

/// @brief Exposes the protected members to the test cases.
class TestStopSignalsHandler : public StopSignalsHandler
{
 public:
  using StopSignalsHandler::get_handled_signals;
  using StopSignalsHandler::get_signal_disposition;
  using StopSignalsHandler::is_ignored;
  using StopSignalsHandler::stop_context;
};

class UTEST_StopSignalsHandler : public Test
{
 public:
  UTEST_StopSignalsHandler()
      : handler{std::make_shared<TestStopSignalsHandler>()},
        appCtx{create_context()}
  {
    restore_the_default_dispositions();
  }

  std::shared_ptr<ApplicationContext> create_context()
  {
    return std::make_shared<ApplicationContext>(argc, argv);
  }

  /// @brief Gives the covered signals the OS default disposition, since a
  /// process inherits the ignored ones of its parent - the nohup tool and the
  /// shell job control do leave some of them ignored.
  void restore_the_default_dispositions()
  {
    for (const int signalNumber : handler->get_handled_signals()) {
      std::signal(signalNumber, SIG_DFL);
    }
  }

  void expect_the_default_dispositions()
  {
    for (const int signalNumber : handler->get_handled_signals()) {
      EXPECT_EQ(std::signal(signalNumber, SIG_DFL), SIG_DFL);
    }
  }

  int argc{0};
  char** argv{nullptr};

  std::shared_ptr<TestStopSignalsHandler> handler;
  std::shared_ptr<ApplicationContext> appCtx;
};

TEST_F(UTEST_StopSignalsHandler, the_handler_belongs_to_the_posix_family)
{
  EXPECT_NE(dynamic_cast<PosixSignalsHandler*>(handler.get()), nullptr);
  EXPECT_NE(dynamic_cast<ASignalsHandler*>(handler.get()), nullptr);
  EXPECT_NE(dynamic_cast<ISignalsHandler*>(handler.get()), nullptr);
}

TEST_F(UTEST_StopSignalsHandler, the_termination_signals_are_covered)
{
  EXPECT_EQ(handler->get_handled_signals().count(SIGINT), 1U);
  EXPECT_EQ(handler->get_handled_signals().count(SIGTERM), 1U);

  // The darknetxx has no configuration to reload, so the SIGHUP of a closed
  // terminal stops it too.
#ifdef SIGHUP
  EXPECT_EQ(handler->get_handled_signals().count(SIGHUP), 1U);
#endif  // SIGHUP

#ifdef SIGTSTP
  EXPECT_EQ(handler->get_handled_signals().count(SIGTSTP), 0U);
#endif  // SIGTSTP
#ifdef SIGCONT
  EXPECT_EQ(handler->get_handled_signals().count(SIGCONT), 0U);
#endif  // SIGCONT
#ifdef SIGSTOP
  EXPECT_EQ(handler->get_handled_signals().count(SIGSTOP), 0U);
#endif  // SIGSTOP
}

TEST_F(UTEST_StopSignalsHandler, the_own_routine_is_provided)
{
  EXPECT_EQ(handler->get_signal_disposition(),
            &TestStopSignalsHandler::stop_context);
}

TEST_F(UTEST_StopSignalsHandler, no_context_error)
{
  EXPECT_FALSE(handler->install({}));

  expect_the_default_dispositions();
}

TEST_F(UTEST_StopSignalsHandler, install_success)
{
  EXPECT_CALL(*appCtx, push_error(_)).Times(0);

  EXPECT_TRUE(handler->install(appCtx));

  EXPECT_FALSE(appCtx->stop.load());
  EXPECT_FALSE(appCtx->pause.load());
}

TEST_F(UTEST_StopSignalsHandler, every_covered_signal_stops_the_context)
{
  for (const int signalNumber : handler->get_handled_signals()) {
    std::shared_ptr<ApplicationContext> signalCtx = create_context();

    EXPECT_TRUE(handler->install(signalCtx));
    EXPECT_FALSE(signalCtx->stop.load());

    std::raise(signalNumber);

    // False positive: the raise does return, since the installed handler
    // does return too.
    // cppcheck-suppress unreachableCode
    EXPECT_TRUE(signalCtx->stop.load());
    EXPECT_FALSE(signalCtx->pause.load());
  }
}

TEST_F(UTEST_StopSignalsHandler, uninstall_restores_the_default_dispositions)
{
  EXPECT_TRUE(handler->install(appCtx));

  handler->uninstall();

  expect_the_default_dispositions();
}

TEST_F(UTEST_StopSignalsHandler,
       the_destructor_restores_the_default_dispositions)
{
  {
    TestStopSignalsHandler scopedHandler;

    EXPECT_TRUE(scopedHandler.install(appCtx));
  }

  expect_the_default_dispositions();
}

TEST_F(UTEST_StopSignalsHandler, the_released_context_stays_untouched)
{
  EXPECT_TRUE(handler->install(appCtx));

  handler->uninstall();

  TestStopSignalsHandler::stop_context(SIGINT);

  EXPECT_FALSE(appCtx->stop.load());
  EXPECT_FALSE(appCtx->pause.load());
}

TEST_F(UTEST_StopSignalsHandler, the_stop_signal_number_is_kept)
{
  EXPECT_TRUE(handler->install(appCtx));

  std::raise(SIGTERM);

  // cppcheck-suppress unreachableCode
  EXPECT_TRUE(appCtx->stop.load());
  EXPECT_EQ(appCtx->stop_signal.load(), SIGTERM);
}

TEST_F(UTEST_StopSignalsHandler, the_copies_of_a_request_are_the_same_request)
{
  EXPECT_TRUE(handler->install(appCtx));

  // The timeout tool, for example, signals the process and then its whole
  // process group, so the copies arriving within the grace period are the
  // very same stop request.
  std::raise(SIGTERM);
  std::raise(SIGTERM);
  std::raise(SIGINT);

  // cppcheck-suppress unreachableCode
  EXPECT_TRUE(appCtx->stop.load());
  EXPECT_EQ(appCtx->stop_signal.load(), SIGTERM);
}

TEST_F(UTEST_StopSignalsHandler, the_routine_stays_installed_after_a_request)
{
  EXPECT_TRUE(handler->install(appCtx));

  std::raise(SIGINT);

  // cppcheck-suppress unreachableCode
  EXPECT_TRUE(appCtx->stop.load());
  EXPECT_EQ(std::signal(SIGINT, SIG_DFL),
            &TestStopSignalsHandler::stop_context);
}

TEST_F(UTEST_StopSignalsHandler, a_later_repeat_terminates_the_process)
{
  // The second Ctrl+C, for example, arrives after the grace period and gets
  // the OS default disposition of the signal.
  EXPECT_EXIT(
      {
        handler->install(appCtx);

        std::raise(SIGINT);

        std::this_thread::sleep_for(std::chrono::milliseconds{
            StopSignalsHandler::GRACE_PERIOD_MS + 100});

        std::raise(SIGINT);

        std::exit(EXIT_SUCCESS);
      },
      KilledBySignal(SIGINT), "");
}

TEST_F(UTEST_StopSignalsHandler, an_inherited_ignore_is_recognized_in_advance)
{
  std::signal(SIGTERM, SIG_IGN);

  EXPECT_TRUE(handler->is_ignored(SIGTERM));
  EXPECT_FALSE(handler->is_ignored(SIGINT));

  EXPECT_TRUE(handler->install(appCtx));

  // the ignore is kept, so the signal does not stop the context
  std::raise(SIGTERM);

  EXPECT_FALSE(appCtx->stop.load());
  EXPECT_EQ(std::signal(SIGTERM, SIG_DFL), SIG_IGN);
}
