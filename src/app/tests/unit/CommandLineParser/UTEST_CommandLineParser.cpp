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

TEST_F(UTEST_CommandLineParser, conversion_flags)
{
  set_args({"--convert-from", "weights", "--convert-src", "yolo.weights",
            "--convert-to", "dxxwjz2", "--convert-dst", "yolo.dxxwjz2",
            "--convert-cfg", "yolo.cfg", "--convert-names", "coco.names"});

  EXPECT_CALL(*appctx, push_error(_)).Times(0);

  EXPECT_TRUE(parser->parse_args(appctx));

  EXPECT_EQ(appctx->convert_from, "weights");
  EXPECT_EQ(appctx->convert_src, "yolo.weights");
  EXPECT_EQ(appctx->convert_to, "dxxwjz2");
  EXPECT_EQ(appctx->convert_dst, "yolo.dxxwjz2");
  EXPECT_EQ(appctx->convert_cfg, "yolo.cfg");
  EXPECT_EQ(appctx->convert_names, "coco.names");
  EXPECT_TRUE(appctx->conversion_requested());
  EXPECT_FALSE(appctx->print_help_and_exit);
}

TEST_F(UTEST_CommandLineParser, conversion_without_the_cfg)
{
  // the dxxwjz1 files keep their structure, the formats are checked by the
  // conversion itself
  set_args({"--convert-to", "weights", "--convert-dst", "a.weights",
            "--convert-from", "dxxwjz1", "--convert-src", "a.dxxwjz1"});

  EXPECT_CALL(*appctx, push_error(_)).Times(0);

  EXPECT_TRUE(parser->parse_args(appctx));

  EXPECT_TRUE(appctx->conversion_requested());
  EXPECT_TRUE(appctx->convert_cfg.empty());
}

TEST_F(UTEST_CommandLineParser, partial_conversion_flags_are_reported)
{
  const std::vector<std::vector<std::string>> partials{
      {"--convert-from", "weights"},
      {"--convert-from", "weights", "--convert-src", "a.weights",
       "--convert-to", "dxxwjz1"},
      {"--convert-dst", "a.dxxwjz1", "--convert-cfg", "a.cfg"},
      {"--convert-to", "dxxwjz2", "--convert-dst", "a.dxxwjz2"},
      {"--convert-src", "a.weights", "--convert-to", "dxxwjz2", "--convert-dst",
       "a.dxxwjz2", "--convert-cfg", "a.cfg"}};

  for (const std::vector<std::string>& partial : partials) {
    appctx = create_context(argc, argv);

    set_args(partial);

    EXPECT_CALL(*appctx,
                push_error(HasSubstr("A conversion requires all of the")))
        .Times(1);

    EXPECT_FALSE(parser->parse_args(appctx));
    EXPECT_TRUE(appctx->print_help_and_exit);
  }
}

TEST_F(UTEST_CommandLineParser,
       conversion_of_no_source_makes_the_network_to_train)
{
  set_args({"--convert-to", "dxxwjz2", "--convert-dst", "net.dxxwjz2",
            "--convert-cfg", "net.cfg"});

  EXPECT_CALL(*appctx, push_error(_)).Times(0);

  EXPECT_TRUE(parser->parse_args(appctx));
  EXPECT_TRUE(appctx->conversion_requested());
}

TEST_F(UTEST_CommandLineParser, draw_chart_of_the_weights_file)
{
  set_args({"--weights", "net.dxxwjz2", "--draw-chart", "chart.png"});

  EXPECT_CALL(*appctx, push_error(_)).Times(0);

  EXPECT_TRUE(parser->parse_args(appctx));
  EXPECT_EQ(appctx->draw_chart, "chart.png");
  EXPECT_TRUE(appctx->chart_requested());

  const std::vector<std::vector<std::string>> wrongs{
      {"--draw-chart", "chart.png"},
      {"--weights", "net.dxxwjz2", "--draw-chart", "chart.png", "--train-cfg",
       "net.cfg"},
      {"--weights", "net.dxxwjz2", "--draw-chart", "chart.png", "--convert-to",
       "dxxwjz2", "--convert-dst", "a.dxxwjz2", "--convert-cfg", "a.cfg"}};

  for (const std::vector<std::string>& wrong : wrongs) {
    appctx = create_context(argc, argv);

    set_args(wrong);

    EXPECT_CALL(*appctx, push_error(HasSubstr("--draw-chart"))).Times(1);

    EXPECT_FALSE(parser->parse_args(appctx));
    EXPECT_TRUE(appctx->print_help_and_exit);
  }
}

TEST_F(UTEST_CommandLineParser, conversion_cfg_alone_is_reported)
{
  set_args({"--convert-cfg", "a.cfg"});

  EXPECT_CALL(*appctx, push_error(HasSubstr("--convert-cfg"))).Times(1);

  EXPECT_FALSE(parser->parse_args(appctx));
  EXPECT_TRUE(appctx->print_help_and_exit);
  EXPECT_FALSE(appctx->conversion_requested());
}

TEST_F(UTEST_CommandLineParser, conversion_names_alone_is_reported)
{
  set_args({"--convert-names", "a.names"});

  EXPECT_CALL(*appctx, push_error(HasSubstr("--convert-names"))).Times(1);

  EXPECT_FALSE(parser->parse_args(appctx));
  EXPECT_TRUE(appctx->print_help_and_exit);
}

TEST_F(UTEST_CommandLineParser, save_format)
{
  EXPECT_EQ(appctx->save_format, "dxxwjz2");

  set_args({"--train-cfg", "net.cfg", "--save-format", "weights"});

  EXPECT_CALL(*appctx, push_error(_)).Times(0);

  EXPECT_TRUE(parser->parse_args(appctx));

  EXPECT_EQ(appctx->save_format, "weights");
}

TEST_F(UTEST_CommandLineParser, conversion_excludes_the_training_and_detection)
{
  for (const std::string& mode :
       {std::string{"--train-cfg"}, std::string{"--detect-cfg"}}) {
    appctx = create_context(argc, argv);

    set_args({mode, "net.cfg", "--convert-from", "weights", "--convert-src",
              "a.weights", "--convert-to", "dxxwjz1", "--convert-dst",
              "a.dxxwjz1"});

    EXPECT_CALL(*appctx, push_error(HasSubstr("can't be combined"))).Times(1);

    EXPECT_FALSE(parser->parse_args(appctx)) << mode;
    EXPECT_TRUE(appctx->print_help_and_exit);
  }
}

TEST_F(UTEST_CommandLineParser, conversion_flags_require_their_values)
{
  for (const std::string& flag :
       {std::string{"--convert-from"}, std::string{"--convert-src"},
        std::string{"--convert-to"}, std::string{"--convert-dst"},
        std::string{"--convert-cfg"}, std::string{"--convert-names"},
        std::string{"--save-format"}, std::string{"--draw-chart"}}) {
    appctx = create_context(argc, argv);

    set_args({flag});

    EXPECT_CALL(*appctx, push_error(HasSubstr("requires the data"))).Times(1);

    EXPECT_FALSE(parser->parse_args(appctx)) << flag;
  }
}
