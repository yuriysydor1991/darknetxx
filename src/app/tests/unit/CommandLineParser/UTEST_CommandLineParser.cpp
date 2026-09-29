#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <string>
#include <vector>

#include "src/app/CommandLineParser.h"

using namespace app;
using namespace testing;

class UTEST_CommandLineParser : public Test
{
 public:
  UTEST_CommandLineParser()
      : parser{std::make_shared<CommandLineParser>()},
        appctx{create_context(argc, argv)}
  {
  }

  std::shared_ptr<ApplicationContext> create_context(int& gargc, char**& gargv)
  {
    return std::make_shared<ApplicationContext>(gargc, gargv);
  }

  void two_args(const char* const secondParam)
  {
    static std::string binaryName{"binaryName"};
    static std::string secondArg;

    static char* customArgv[] = {binaryName.data(), secondArg.data()};

    secondArg = secondParam;

    customArgv[1] = secondArg.data();

    argc = 2;
    argv = customArgv;
  }

  void set_args(const std::vector<std::string>& params)
  {
    storedArgs.clear();
    storedArgs.emplace_back("binaryName");
    storedArgs.insert(storedArgs.end(), params.begin(), params.end());

    storedArgv.clear();

    for (std::string& arg : storedArgs) {
      storedArgv.push_back(arg.data());
    }

    argc = static_cast<int>(storedArgv.size());
    argv = storedArgv.data();
  }

  int argc{0};
  char** argv{nullptr};

  std::vector<std::string> storedArgs;
  std::vector<char*> storedArgv;

  std::shared_ptr<CommandLineParser> parser;
  std::shared_ptr<ApplicationContext> appctx;
};

TEST_F(UTEST_CommandLineParser, no_context_error)
{
  EXPECT_FALSE(parser->parse_args({}));
}

TEST_F(UTEST_CommandLineParser, empty_context)
{
  EXPECT_TRUE(parser->parse_args(appctx));
}

TEST_F(UTEST_CommandLineParser, help_short)
{
  two_args("-h");

  EXPECT_CALL(*appctx, push_error(_)).Times(0);

  EXPECT_TRUE(parser->parse_args(appctx));

  EXPECT_TRUE(appctx->print_help_and_exit);
  EXPECT_FALSE(appctx->print_version_and_exit);
  EXPECT_TRUE(appctx->errors.empty());
}

TEST_F(UTEST_CommandLineParser, help_long)
{
  two_args("--help");

  EXPECT_CALL(*appctx, push_error(_)).Times(0);

  EXPECT_TRUE(parser->parse_args(appctx));

  EXPECT_TRUE(appctx->print_help_and_exit);
  EXPECT_FALSE(appctx->print_version_and_exit);
  EXPECT_TRUE(appctx->errors.empty());
}

TEST_F(UTEST_CommandLineParser, version_short)
{
  two_args("-v");

  EXPECT_CALL(*appctx, push_error(_)).Times(0);

  EXPECT_TRUE(parser->parse_args(appctx));

  EXPECT_FALSE(appctx->print_help_and_exit);
  EXPECT_TRUE(appctx->print_version_and_exit);
  EXPECT_TRUE(appctx->errors.empty());
}

TEST_F(UTEST_CommandLineParser, version_long)
{
  two_args("--version");

  EXPECT_CALL(*appctx, push_error(_)).Times(0);

  EXPECT_TRUE(parser->parse_args(appctx));

  EXPECT_FALSE(appctx->print_help_and_exit);
  EXPECT_TRUE(appctx->print_version_and_exit);
  EXPECT_TRUE(appctx->errors.empty());
}

TEST_F(UTEST_CommandLineParser, unknown_flag)
{
  static constexpr const char* const unknownFlag = "--unknown";
  static std::string unknownFlagStr{unknownFlag};
  static const std::string expectedError =
      std::string{"Unknown parameter: "} + unknownFlagStr;

  two_args(unknownFlag);

  EXPECT_CALL(*appctx, push_error(expectedError)).Times(1);

  EXPECT_FALSE(parser->parse_args(appctx));

  EXPECT_TRUE(appctx->print_help_and_exit);
  EXPECT_FALSE(appctx->print_version_and_exit);
  EXPECT_TRUE(appctx->errors.empty());
}

TEST_F(UTEST_CommandLineParser, ext_output_is_a_flag)
{
  set_args({"--ext-output", "--image", "x.jpg"});

  EXPECT_CALL(*appctx, push_error(_)).Times(0);

  EXPECT_TRUE(parser->parse_args(appctx));

  EXPECT_TRUE(appctx->ext_output);
  EXPECT_EQ(appctx->image, "x.jpg");
  EXPECT_FALSE(appctx->print_help_and_exit);
}

TEST_F(UTEST_CommandLineParser, ext_output_as_the_last_param)
{
  set_args({"--ext-output"});

  EXPECT_CALL(*appctx, push_error(_)).Times(0);

  EXPECT_TRUE(parser->parse_args(appctx));

  EXPECT_TRUE(appctx->ext_output);
  EXPECT_FALSE(appctx->print_help_and_exit);
  EXPECT_FALSE(appctx->print_version_and_exit);
}

TEST_F(UTEST_CommandLineParser, not_supported_params_are_rejected)
{
  // the value of the parameter is not taken for an unknown parameter
  set_args({"--prefix", "abc", "--dont-show"});

  EXPECT_CALL(
      *appctx,
      push_error(std::string{"The parameter --prefix is not supported yet"}))
      .Times(1);

  EXPECT_FALSE(parser->parse_args(appctx));

  EXPECT_TRUE(appctx->print_help_and_exit);
}

TEST_F(UTEST_CommandLineParser, show_reverts_dont_show)
{
  set_args({"--dont-show", "--show"});

  EXPECT_CALL(*appctx, push_error(_)).Times(0);

  EXPECT_TRUE(parser->parse_args(appctx));

  EXPECT_FALSE(appctx->dont_show);
}

TEST_F(UTEST_CommandLineParser, missing_value_prints_help)
{
  set_args({"--train-cfg"});

  EXPECT_CALL(*appctx, push_error(_)).Times(1);

  EXPECT_FALSE(parser->parse_args(appctx));

  EXPECT_TRUE(appctx->print_help_and_exit);
  EXPECT_FALSE(appctx->print_version_and_exit);
}

TEST_F(UTEST_CommandLineParser, malformed_number_is_reported)
{
  set_args({"--thresh", "abc"});

  EXPECT_CALL(*appctx,
              push_error(std::string{"Invalid value 'abc' of the parameter "
                                     "--thresh"}))
      .Times(1);

  EXPECT_FALSE(parser->parse_args(appctx));

  EXPECT_TRUE(appctx->print_help_and_exit);
}

TEST_F(UTEST_CommandLineParser, number_with_trailing_garbage_is_reported)
{
  set_args({"--threads", "4x"});

  EXPECT_CALL(*appctx, push_error(_)).Times(1);

  EXPECT_FALSE(parser->parse_args(appctx));

  EXPECT_TRUE(appctx->print_help_and_exit);
}

TEST_F(UTEST_CommandLineParser, negative_count_is_reported)
{
  set_args({"--net-resize-nth", "-1"});

  EXPECT_CALL(*appctx, push_error(_)).Times(1);

  EXPECT_FALSE(parser->parse_args(appctx));

  EXPECT_EQ(appctx->net_nth_resize, 10U);
}

TEST_F(UTEST_CommandLineParser, out_of_range_number_is_reported)
{
  set_args({"--points", "99999999999999999999"});

  EXPECT_CALL(*appctx, push_error(_)).Times(1);

  EXPECT_FALSE(parser->parse_args(appctx));
}

TEST_F(UTEST_CommandLineParser, numbers_are_parsed)
{
  set_args({"--thresh", "0.5", "--threads", "4", "--stop-less-avg-loss", "0.25",
            "--points", "101"});

  EXPECT_CALL(*appctx, push_error(_)).Times(0);

  EXPECT_TRUE(parser->parse_args(appctx));

  EXPECT_FLOAT_EQ(appctx->thresh, 0.5f);
  EXPECT_EQ(appctx->threads, 4U);
  EXPECT_DOUBLE_EQ(appctx->stop_less_avg_loss, 0.25);
  EXPECT_EQ(appctx->points, 101);
}

TEST_F(UTEST_CommandLineParser, malformed_gpus_list_is_reported)
{
  set_args({"--gpus", "0,,1"});

  EXPECT_CALL(*appctx, push_error(_)).Times(1);

  EXPECT_FALSE(parser->parse_args(appctx));

  EXPECT_TRUE(appctx->gpus.empty());
}

TEST_F(UTEST_CommandLineParser, gpus_list)
{
  set_args({"--gpus", "0,1"});

  EXPECT_CALL(*appctx, push_error(_)).Times(0);

  EXPECT_TRUE(parser->parse_args(appctx));

  EXPECT_EQ(appctx->gpus, "0,1");
}

TEST_F(UTEST_CommandLineParser, names_file)
{
  set_args({"--names", "coco.names"});

  EXPECT_CALL(*appctx, push_error(_)).Times(0);

  EXPECT_TRUE(parser->parse_args(appctx));

  EXPECT_EQ(appctx->names, "coco.names");
}

TEST_F(UTEST_CommandLineParser, flags)
{
  set_args({"--benchmark-layers", "--benchmark"});

  EXPECT_CALL(*appctx, push_error(_)).Times(0);

  EXPECT_TRUE(parser->parse_args(appctx));

  EXPECT_TRUE(appctx->benchmark_layers);
  EXPECT_TRUE(appctx->benchmark);
}

TEST_F(UTEST_CommandLineParser, train_and_detect_cfgs_are_exclusive)
{
  set_args({"--train-cfg", "train.cfg", "--detect-cfg", "detect.cfg"});

  EXPECT_CALL(*appctx, push_error(_)).Times(1);

  EXPECT_FALSE(parser->parse_args(appctx));

  EXPECT_TRUE(appctx->print_help_and_exit);
}
