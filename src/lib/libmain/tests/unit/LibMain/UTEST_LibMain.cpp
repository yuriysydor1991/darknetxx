#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>

#include "LibraryContext.h"
#include "src/OMP/OMPController.h"
#include "src/darknet-adaptor/DarknetController.h"
#include "src/lib/libmain/LibMain.h"

using darknet_adaptor::DarknetController;
using darknetxx011::LibraryContext;
using darknetxx011::LibraryContextPtr;
using darknetxx_impl::LibMain;
using ompi::OMPController;

class UTEST_LibMain : public testing::Test
{
 public:
  ~UTEST_LibMain() override
  {
    DarknetController::on_perform = nullptr;
    OMPController::on_adjust = nullptr;
  }

  LibraryContextPtr ctx{std::make_shared<LibraryContext>()};
  LibMain lib;
};

TEST_F(UTEST_LibMain, no_context_error) { EXPECT_FALSE(lib.libcall({})); }

TEST_F(UTEST_LibMain, the_threads_are_adjusted_before_the_work)
{
  bool adjusted{false};

  OMPController::on_adjust = [&](LibraryContextPtr given) {
    EXPECT_EQ(given, ctx);
    adjusted = true;
    return true;
  };

  DarknetController::on_perform = [&](LibraryContextPtr given) {
    EXPECT_EQ(given, ctx);
    EXPECT_TRUE(adjusted);
    return true;
  };

  EXPECT_TRUE(lib.libcall(ctx));
}

TEST_F(UTEST_LibMain, failed_threads_adjust_performs_nothing)
{
  OMPController::on_adjust = [](LibraryContextPtr) { return false; };
  DarknetController::on_perform = [](LibraryContextPtr) {
    ADD_FAILURE() << "The work is performed";
    return true;
  };

  EXPECT_FALSE(lib.libcall(ctx));
}

TEST_F(UTEST_LibMain, failed_work_is_the_failure)
{
  DarknetController::on_perform = [](LibraryContextPtr) { return false; };

  EXPECT_FALSE(lib.libcall(ctx));
}
