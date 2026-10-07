#include <gtest/gtest.h>

#include <atomic>

#include "LibraryContext.h"

using darknetxx011::LibraryContext;

TEST(UTEST_LibraryContext, no_stop_is_requested_at_first)
{
  EXPECT_FALSE(LibraryContext{}.stop_requested());
}

TEST(UTEST_LibraryContext, the_stop_request_is_withdrawn)
{
  LibraryContext ctx;

  ctx.request_stop();

  EXPECT_TRUE(ctx.stop_requested());

  ctx.withdraw_stop();

  EXPECT_FALSE(ctx.stop_requested());
}

TEST(UTEST_LibraryContext, the_raised_flag_requests_the_stop)
{
  std::atomic_bool flag{false};
  LibraryContext ctx;

  ctx.stop_with(flag);

  EXPECT_FALSE(ctx.stop_requested());

  flag.store(true);

  EXPECT_TRUE(ctx.stop_requested());

  // the flag is the one of the context user, which lowers it
  ctx.withdraw_stop();

  EXPECT_TRUE(ctx.stop_requested());
}

TEST(UTEST_LibraryContext, any_conversion_parameter_requests_the_conversion)
{
  LibraryContext ctx;

  EXPECT_FALSE(ctx.conversion_requested());

  ctx.convert_dst = "net.dxxwjz1";

  EXPECT_TRUE(ctx.conversion_requested());
}
