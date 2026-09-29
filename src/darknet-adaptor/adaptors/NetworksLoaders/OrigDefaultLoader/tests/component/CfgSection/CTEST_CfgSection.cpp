#include <gtest/gtest.h>

#include <cstdio>
#include <fstream>
#include <string>

#include "src/darknet-adaptor/adaptors/NetworkError.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/OrigDefaultLoader/CfgSection.h"

using namespace testing;

using darknet_adaptor::adaptors::NetworkError;
using darknet_adaptor::loaders::CfgSection;
using darknet_adaptor::loaders::CfgSections;

class CTEST_CfgSection : public Test
{
 public:
  CTEST_CfgSection()
  {
    const TestInfo* const info = UnitTest::GetInstance()->current_test_info();
    cfg = std::string{CTEST_CfgSection_DATA_DIR} + "/" + info->name() + ".cfg";
  }

  ~CTEST_CfgSection() { std::remove(cfg.c_str()); }

  /// @brief Writes the cfg file of the content and reads it's sections.
  CfgSections read(const std::string& content)
  {
    {
      std::ofstream out{cfg, std::ios::binary};
      out << content;
    }

    return CfgSection::read_cfg(cfg);
  }

  std::string cfg;
};

TEST_F(CTEST_CfgSection, read_cfg_keeps_the_sections_in_the_file_order)
{
  CfgSections sections = read(
      "[net]\nbatch=2\n[convolutional]\nfilters=4\n[yolo]\n[maxpool]\n"
      "size=2\n");

  ASSERT_EQ(sections.size(), 4U);
  EXPECT_EQ(sections[0].type(), "[net]");
  EXPECT_EQ(sections[1].type(), "[convolutional]");
  EXPECT_EQ(sections[2].type(), "[yolo]");
  EXPECT_EQ(sections[3].type(), "[maxpool]");

  // every section keeps it's own options
  EXPECT_STREQ(sections[0].find("batch"), "2");
  EXPECT_EQ(sections[0].find("filters"), nullptr);
  EXPECT_STREQ(sections[1].find("filters"), "4");
  EXPECT_STREQ(sections[3].find("size"), "2");
}

TEST_F(CTEST_CfgSection, read_cfg_removes_every_whitespace)
{
  CfgSections sections = read(
      " [ net ] \r\n"
      "\tbatch = 64\r\n"
      "anchors = 10,14,  23,27\t\n"
      "policy=\t steps \n");

  ASSERT_EQ(sections.size(), 1U);
  EXPECT_EQ(sections[0].type(), "[net]");
  EXPECT_STREQ(sections[0].find("batch"), "64");
  EXPECT_STREQ(sections[0].find("anchors"), "10,14,23,27");
  EXPECT_STREQ(sections[0].find("policy"), "steps");
}

TEST_F(CTEST_CfgSection, read_cfg_skips_the_comments_and_the_empty_lines)
{
  CfgSections sections = read(
      "# the network\n"
      "; the old style comment\n"
      "\n"
      "   \n"
      "[net]\n"
      "#batch=1\n"
      "batch=8 # the inline comment stays in the value\n"
      ";batch=2\n"
      "\n");

  ASSERT_EQ(sections.size(), 1U);
  EXPECT_STREQ(sections[0].find("batch"), "8#theinlinecommentstaysinthevalue");
  EXPECT_EQ(sections[0].find_int("batch", 0), 8);
}

TEST_F(CTEST_CfgSection, read_cfg_of_the_last_line_without_the_newline)
{
  CfgSections sections = read("[net]\nwidth=416");

  ASSERT_EQ(sections.size(), 1U);
  EXPECT_EQ(sections[0].find_int("width", 0), 416);
}

TEST_F(CTEST_CfgSection, read_cfg_of_an_empty_file_has_no_sections)
{
  EXPECT_TRUE(read("").empty());
  EXPECT_TRUE(read("# the comments only\n\n;\n").empty());
}

TEST_F(CTEST_CfgSection, read_cfg_of_a_missing_file_throws)
{
  EXPECT_THROW(CfgSection::read_cfg(cfg + ".missing"), NetworkError);
  EXPECT_THROW(CfgSection::read_cfg(""), NetworkError);
}

TEST_F(CTEST_CfgSection, read_cfg_of_an_option_before_the_first_section_throws)
{
  // the original dereferenced the NULL current section
  EXPECT_THROW(read("batch=1\n[net]\nwidth=8\n"), NetworkError);
}

TEST_F(CTEST_CfgSection, read_cfg_skips_the_options_without_the_value)
{
  CfgSections sections = read("[net]\nlearning_rate=\nbatch=4\n");

  ASSERT_EQ(sections.size(), 1U);
  EXPECT_EQ(sections[0].find("learning_rate"), nullptr);
  EXPECT_FLOAT_EQ(sections[0].find_float("learning_rate", 0.5f), 0.5f);
  EXPECT_EQ(sections[0].find_int("batch", 0), 4);
}

TEST_F(CTEST_CfgSection, add_splits_the_line_at_the_first_equal_sign)
{
  CfgSection section{"[net]"};

  EXPECT_TRUE(section.add("key=value"));
  EXPECT_TRUE(section.add("expression=a=b"));
  EXPECT_FALSE(section.add("empty="));

  EXPECT_STREQ(section.find("key"), "value");
  EXPECT_STREQ(section.find("expression"), "a=b");
  EXPECT_EQ(section.find("empty"), nullptr);
}

TEST_F(CTEST_CfgSection, add_of_the_line_without_the_equal_sign_is_a_null_value)
{
  CfgSection section{"[net]"};

  // the read_option() inserted the key of the NULL value
  EXPECT_TRUE(section.add("flag"));

  EXPECT_EQ(section.find("flag"), nullptr);
  EXPECT_EQ(section.find_int("flag", 7), 7);
  EXPECT_STREQ(section.find_str("flag", "default"), "default");
}

TEST_F(CTEST_CfgSection, find_takes_the_first_option_of_the_key)
{
  CfgSection section{"[net]"};

  ASSERT_TRUE(section.add("batch=1"));
  ASSERT_TRUE(section.add("batch=2"));

  EXPECT_STREQ(section.find("batch"), "1");
  EXPECT_EQ(section.find_int("batch", 0), 1);
  EXPECT_EQ(section.find("Batch"), nullptr);
  EXPECT_EQ(section.find("batc"), nullptr);
}

TEST_F(CTEST_CfgSection, the_numbers_are_converted_like_atoi_and_atof)
{
  CfgSection section{"[net]"};

  ASSERT_TRUE(section.add("prefix=12abc"));
  ASSERT_TRUE(section.add("text=abc"));
  ASSERT_TRUE(section.add("negative=-3"));
  ASSERT_TRUE(section.add("fraction=.5"));
  ASSERT_TRUE(section.add("exponent=1e-3"));
  ASSERT_TRUE(section.add("commented=4#comment"));
  ASSERT_TRUE(section.add("real=2.9"));

  EXPECT_EQ(section.find_int("prefix", -1), 12);
  EXPECT_EQ(section.find_int("text", -1), 0);
  EXPECT_EQ(section.find_int("negative", 0), -3);
  EXPECT_EQ(section.find_int("commented", 0), 4);
  EXPECT_EQ(section.find_int_quiet("real", 0), 2);

  EXPECT_FLOAT_EQ(section.find_float("fraction", 0), 0.5f);
  EXPECT_FLOAT_EQ(section.find_float_quiet("exponent", 0), 0.001f);
  EXPECT_FLOAT_EQ(section.find_float("text", 1), 0.0f);
  EXPECT_FLOAT_EQ(section.find_float("negative", 0), -3.0f);
}

TEST_F(CTEST_CfgSection, the_missing_keys_take_the_defaults)
{
  CfgSection section{"[yolo]"};
  const char* const def = "default";

  EXPECT_EQ(section.find_str("missing", def), def);
  EXPECT_EQ(section.find_str_quiet("missing", def), def);
  EXPECT_EQ(section.find_str("missing", nullptr), nullptr);
  EXPECT_EQ(section.find_int("missing", 42), 42);
  EXPECT_EQ(section.find_int_quiet("missing", -42), -42);
  EXPECT_FLOAT_EQ(section.find_float("missing", 0.25f), 0.25f);
  EXPECT_FLOAT_EQ(section.find_float_quiet("missing", -0.25f), -0.25f);
}

TEST_F(CTEST_CfgSection, the_values_of_the_read_sections_stay_valid)
{
  CfgSections sections = read("[net]\nnames=data/obj.names\n[yolo]\n");

  const char* const names = sections[0].find_str("names", nullptr);

  // the lookups of the other sections and keys keep the value in place
  sections[1].find_int("classes", 20);
  sections[0].find_int("batch", 1);

  EXPECT_STREQ(names, "data/obj.names");
}

TEST_F(CTEST_CfgSection, unused_logs_the_options_no_lookup_asked_for)
{
  CfgSection section{"[convolutional]"};

  ASSERT_TRUE(section.add("filters=16"));
  ASSERT_TRUE(section.add("typo_option=1"));
  ASSERT_TRUE(section.add("flag"));

  section.find_int("filters", 1);

  internal::CaptureStderr();
  section.unused();
  const std::string warnings = internal::GetCapturedStderr();

  EXPECT_NE(warnings.find("Unused field: 'typo_option = 1'"), std::string::npos)
      << warnings;
  EXPECT_NE(warnings.find("Unused field: 'flag = (null)'"), std::string::npos)
      << warnings;
  EXPECT_EQ(warnings.find("filters"), std::string::npos) << warnings;

  // a lookup of the NULL value option marks it used as well
  section.find("typo_option");
  section.find("flag");

  internal::CaptureStderr();
  section.unused();
  EXPECT_EQ(internal::GetCapturedStderr().find("Unused field"),
            std::string::npos);
}
