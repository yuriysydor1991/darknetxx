#include <gtest/gtest.h>

#include <memory>

#include "LibraryContext.h"
#include "src/app/ApplicationContext.h"
#include "src/converters/ApplicationContext2LibraryContext.h"

using converters::ApplicationContext2LibraryContext;
using darknetxx011::LibraryContext;

class UTEST_ApplicationContext2LibraryContext : public testing::Test
{
 public:
  int argc{0};
  char** argv{nullptr};
  app::AppCtxPtr appctx{std::make_shared<app::ApplicationContext>(argc, argv)};
  darknetxx011::LibraryContextPtr libctx{std::make_shared<LibraryContext>()};
  ApplicationContext2LibraryContext converter;
};

TEST_F(UTEST_ApplicationContext2LibraryContext, no_context_is_the_failure)
{
  EXPECT_FALSE(converter.convert({}, libctx));
  EXPECT_FALSE(converter.convert(appctx, {}));
}

TEST_F(UTEST_ApplicationContext2LibraryContext, the_command_line_is_transferred)
{
  appctx->train_cfg = "train.cfg";
  appctx->latest_weights = "net.weights";
  appctx->threads = 3;
  appctx->thresh = 0.75f;
  appctx->dont_show = true;
  appctx->reload_data = false;
  appctx->save_format = "dxxwjz1";
  appctx->convert_from = "weights";
  appctx->convert_cfg = "net.cfg";
  appctx->convert_names = "net.names";

  ASSERT_TRUE(converter.convert(appctx, libctx));

  EXPECT_EQ(libctx->train_cfg, "train.cfg");
  EXPECT_EQ(libctx->latest_weights, "net.weights");
  EXPECT_EQ(libctx->threads, 3U);
  EXPECT_FLOAT_EQ(libctx->thresh, 0.75f);
  EXPECT_TRUE(libctx->dont_show);
  EXPECT_FALSE(libctx->reload_data);
  EXPECT_EQ(libctx->save_format, "dxxwjz1");
  EXPECT_EQ(libctx->convert_from, "weights");
  EXPECT_EQ(libctx->convert_cfg, "net.cfg");
  EXPECT_EQ(libctx->convert_names, "net.names");
}

TEST_F(UTEST_ApplicationContext2LibraryContext, the_stop_flag_stops_the_library)
{
  ASSERT_TRUE(converter.convert(appctx, libctx));

  EXPECT_FALSE(libctx->stop_requested());

  appctx->stop.store(true);

  EXPECT_TRUE(libctx->stop_requested());
}
