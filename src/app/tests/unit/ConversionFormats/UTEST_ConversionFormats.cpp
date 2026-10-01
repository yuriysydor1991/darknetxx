#include <gtest/gtest.h>

#include <string>
#include <vector>

#include "src/app/converter/ConversionFormats.h"

using namespace app::converter;

TEST(UTEST_ConversionFormats, names_are_the_help_message_order)
{
  EXPECT_EQ(ConversionFormats::names(),
            (std::vector<std::string>{"weights", "dxxwjz1"}));
}

TEST(UTEST_ConversionFormats, names_resolve_to_the_formats)
{
  ConversionFormat format = ConversionFormat::dxxwjz1;

  ASSERT_TRUE(ConversionFormats::parse("weights", format));
  EXPECT_EQ(format, ConversionFormat::weights);

  ASSERT_TRUE(ConversionFormats::parse("dxxwjz1", format));
  EXPECT_EQ(format, ConversionFormat::dxxwjz1);
}

TEST(UTEST_ConversionFormats, unknown_names_leave_the_format_untouched)
{
  for (const std::string& name : std::vector<std::string>{
           "", "Weights", "WEIGHTS", "dxxwjz", "dxxwjz2", " weights", "json"}) {
    ConversionFormat format = ConversionFormat::dxxwjz1;

    EXPECT_FALSE(ConversionFormats::parse(name, format)) << name;
    EXPECT_EQ(format, ConversionFormat::dxxwjz1) << name;
  }
}

TEST(UTEST_ConversionFormats, every_format_is_described)
{
  for (const std::string& name : ConversionFormats::names()) {
    EXPECT_FALSE(ConversionFormats::description(name).empty()) << name;
  }

  EXPECT_NE(ConversionFormats::description("weights").find("cfg"),
            std::string::npos);
  EXPECT_TRUE(ConversionFormats::description("unknown").empty());
}
