#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <fstream>
#include <functional>
#include <locale>
#include <set>
#include <string>
#include <thread>

#include "GroupingNumpunct.h"
#include "src/log/default-logger/DefaultLogger.h"

using namespace testing;
using namespace default_logger;

class CTEST_DefaultLogger : public Test
{
 public:
  inline static const std::string test_default_log_file =
      CTEST_DefaultLogger_DATA_DIR "/CTEST_DefaultLogger.log";
  inline static const unsigned short& default_log_level =
      DefaultLogger::LVL_TRACE;

  CTEST_DefaultLogger() { clear_default_log_file(); }
  ~CTEST_DefaultLogger() { clear_default_log_file(); }

  void default_log_lvl_init(const unsigned short& lvl)
  {
    DefaultLogger::init(test_default_log_file, lvl, true);
  }

  void default_log_init(const bool print = true)
  {
    DefaultLogger::init(test_default_log_file, default_log_level, print);
  }

  std::string get_default_log_file_contents()
  {
    std::fstream log{test_default_log_file.c_str(), std::fstream::in};

    std::string rt;
    char c{0};

    while (log.get(c)) {
      rt += c;
    }

    log.close();

    return rt;
  }

  void clear_default_log_file()
  {
    std::fstream log{test_default_log_file.c_str(),
                     std::fstream::out | std::fstream::trunc};

    log.close();
  }

  const std::set<unsigned short> levelsSet{
      DefaultLogger::LVL_ERROR, DefaultLogger::LVL_WARNING,
      DefaultLogger::LVL_INFO, DefaultLogger::LVL_DEBUG,
      DefaultLogger::LVL_TRACE};
};

TEST_F(CTEST_DefaultLogger, nonempty_defaultLogDateFormat)
{
  EXPECT_FALSE(std::string{DefaultLogger::defaultLogDateFormat}.empty());
}

TEST_F(CTEST_DefaultLogger, nonempty_default_log_name)
{
  EXPECT_FALSE(DefaultLogger::default_log_name.empty());
}

TEST_F(CTEST_DefaultLogger, no_equal_levels)
{
  EXPECT_GE(levelsSet.size(), 5U);

  for (auto iter = levelsSet.begin(); iter != levelsSet.end(); ++iter) {
    auto cval = *iter;
    auto next = iter;
    for (auto jter = ++next; jter != levelsSet.end(); ++jter) {
      EXPECT_NE(*jter, cval);
    }
  }
}

TEST_F(CTEST_DefaultLogger, first_error_log)
{
  static const std::string test_str = "first log test 0123-";

  default_log_init();

  DefaultLogger::log(DefaultLogger::LVL_ERROR, test_str);

  const auto logs = get_default_log_file_contents();

  EXPECT_FALSE(logs.empty());

  EXPECT_THAT(logs, EndsWith(test_str + "\n"));
}

TEST_F(CTEST_DefaultLogger, trace_with_max_debug_log_absent)
{
  default_log_lvl_init(DefaultLogger::LVL_DEBUG);

  DefaultLogger::log(DefaultLogger::LVL_TRACE, "Expect absent");

  const auto logs = get_default_log_file_contents();

  EXPECT_TRUE(logs.empty());
  EXPECT_EQ(logs, std::string{});
}

TEST_F(CTEST_DefaultLogger, debug_with_max_info_log_absent)
{
  default_log_lvl_init(DefaultLogger::LVL_INFO);

  DefaultLogger::log(DefaultLogger::LVL_DEBUG, "Expect absent");

  const auto logs = get_default_log_file_contents();

  EXPECT_TRUE(logs.empty());
  EXPECT_EQ(logs, std::string{});
}

TEST_F(CTEST_DefaultLogger, info_with_max_warning_log_absent)
{
  default_log_lvl_init(DefaultLogger::LVL_WARNING);

  DefaultLogger::log(DefaultLogger::LVL_INFO, "Expect absent");

  const auto logs = get_default_log_file_contents();

  EXPECT_TRUE(logs.empty());
  EXPECT_EQ(logs, std::string{});
}

TEST_F(CTEST_DefaultLogger, warning_with_max_error_log_absent)
{
  default_log_lvl_init(DefaultLogger::LVL_ERROR);

  DefaultLogger::log(DefaultLogger::LVL_WARNING, "Expect absent");

  const auto logs = get_default_log_file_contents();

  EXPECT_TRUE(logs.empty());
  EXPECT_EQ(logs, std::string{});
}

TEST_F(CTEST_DefaultLogger, warning_with_max_info_log_present)
{
  static const std::string expect = "expect present";

  default_log_lvl_init(DefaultLogger::LVL_INFO);

  DefaultLogger::log(DefaultLogger::LVL_WARNING, expect);

  const auto logs = get_default_log_file_contents();

  EXPECT_FALSE(logs.empty());
  EXPECT_THAT(logs, EndsWith(expect + "\n"));
}

TEST_F(CTEST_DefaultLogger, error_with_max_warning_log_present)
{
  static const std::string expect = "expect present";

  default_log_lvl_init(DefaultLogger::LVL_WARNING);

  DefaultLogger::log(DefaultLogger::LVL_ERROR, expect);

  const auto logs = get_default_log_file_contents();

  EXPECT_FALSE(logs.empty());
  EXPECT_THAT(logs, EndsWith(expect + "\n"));
}

// The presence of the lower severities at the matching level proves that their
// absence above is the level filtering and not an unflushed buffer.
TEST_F(CTEST_DefaultLogger, info_with_max_info_log_present)
{
  static const std::string expect = "expect present";

  default_log_lvl_init(DefaultLogger::LVL_INFO);

  DefaultLogger::log(DefaultLogger::LVL_INFO, expect);

  EXPECT_THAT(get_default_log_file_contents(), EndsWith(expect + "\n"));
}

TEST_F(CTEST_DefaultLogger, debug_with_max_debug_log_present)
{
  static const std::string expect = "expect present";

  default_log_lvl_init(DefaultLogger::LVL_DEBUG);

  DefaultLogger::log(DefaultLogger::LVL_DEBUG, expect);

  EXPECT_THAT(get_default_log_file_contents(), EndsWith(expect + "\n"));
}

TEST_F(CTEST_DefaultLogger, trace_with_max_trace_log_present)
{
  static const std::string expect = "expect present";

  default_log_lvl_init(DefaultLogger::LVL_TRACE);

  DefaultLogger::log(DefaultLogger::LVL_TRACE, expect);

  EXPECT_THAT(get_default_log_file_contents(), EndsWith(expect + "\n"));
}

TEST_F(CTEST_DefaultLogger, prepare_buff_formats_exactly)
{
  const std::string formatted = DefaultLogger::prepare_buff("%d-%s", 42, "x");

  EXPECT_EQ(formatted, std::string{"42-x"});
  EXPECT_EQ(formatted.size(), 4U);
}

TEST_F(CTEST_DefaultLogger, prepare_buff_formats_a_long_message)
{
  const std::string longArg(300000U, 'a');

  const std::string formatted =
      DefaultLogger::prepare_buff("[%s]", longArg.c_str());

  EXPECT_EQ(formatted.size(), longArg.size() + 2U);
  EXPECT_EQ(formatted, "[" + longArg + "]");
}

TEST_F(CTEST_DefaultLogger, unopenable_log_file_is_not_fatal)
{
  DefaultLogger::logfile("/nonexistent-dir/for-sure/log.log");

  // the console only logging keeps working
  default_log_init();

  static const std::string expect = "expect present";

  DefaultLogger::log(DefaultLogger::LVL_ERROR, expect);

  EXPECT_THAT(get_default_log_file_contents(), EndsWith(expect + "\n"));
}

TEST_F(CTEST_DefaultLogger, log_fields_ignore_the_application_global_locale)
{
  const std::locale previous = std::locale::global(
      std::locale{std::locale::classic(), new GroupingNumpunct});

  default_log_init();

  DefaultLogger::log(DefaultLogger::LVL_ERROR, "the message");

  std::locale::global(previous);

  // no group separators in the thread id and the timestamp of the logger
  const auto logs = get_default_log_file_contents();

  EXPECT_THAT(logs, Not(HasSubstr(",")));
  EXPECT_THAT(logs, ContainsRegex(" ERR [0-9]+ the message"));
}

TEST_F(CTEST_DefaultLogger, multithread_logs)
{
  static constexpr const unsigned int iters_per_thread = 1000U;

  default_log_init(false);

  std::set<std::shared_ptr<std::thread>> ths;

  const auto concurencyLog = []() {
    for (auto jter = 0U; jter < iters_per_thread; ++jter) {
      DefaultLogger::log(DefaultLogger::LVL_ERROR,
                         "Concurency thread log msg #" + std::to_string(jter));
    }
  };

  for (auto iter = 0U; iter < std::thread::hardware_concurrency(); ++iter) {
    ths.insert(std::make_shared<std::thread>(concurencyLog));
  }

  for (auto& th : ths) {
    if (th->joinable()) {
      th->join();
    }
  }

  ths.clear();

  auto logs = get_default_log_file_contents();

  const unsigned int expectedLogsLines =
      std::thread::hardware_concurrency() * iters_per_thread;

  const auto newlines = std::count_if(logs.begin(), logs.end(),
                                      [](const char& c) { return c == '\n'; });

  EXPECT_EQ(newlines, expectedLogsLines);

  while (!logs.empty()) {
    auto lineEnd = std::find(logs.begin(), logs.end(), '\n');

    std::string line{logs.begin(), lineEnd};

    if (lineEnd < logs.end()) {
      lineEnd++;
    }

    logs.erase(logs.begin(), lineEnd);

    EXPECT_THAT(line, ContainsRegex(" Concurency thread log msg #[0-9]{1,3}$"));
  }
}
