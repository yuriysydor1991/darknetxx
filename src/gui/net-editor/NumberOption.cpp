#include "src/gui/net-editor/NumberOption.h"

#include <cctype>
#include <cmath>
#include <iomanip>
#include <locale>
#include <sstream>

namespace net_editor
{

NumberOption::NumberOption(std::string nkey, std::string ndef,
                           std::string nabout, const bool nintegers,
                           const bool nlist, Range nrange)
    : AOption{std::move(nkey), std::move(ndef), std::move(nabout)},
      mintegers{nintegers},
      mlist{nlist},
      mrange{std::move(nrange)}
{
}

bool NumberOption::typable(const std::string& text) const
{
  const std::string allowed =
      std::string{"-"} + (mlist ? "," : "") + (mintegers ? "" : ".eE+");

  for (const char symbol : text) {
    if (std::isdigit(static_cast<unsigned char>(symbol)) == 0 &&
        allowed.find(symbol) == std::string::npos) {
      return false;
    }
  }

  return true;
}

std::string NumberOption::limits(const OptionContext& ctx) const
{
  const std::string kind =
      mintegers ? (mlist ? "comma separated integers" : "an integer")
                : (mlist ? "comma separated real numbers" : "a real number");
  const std::string bounds = range_text(range(ctx));
  const std::string further = rule(ctx);

  return kind + (bounds.empty() ? "" : " " + bounds) +
         (further.empty() ? "" : ", " + further);
}

std::string NumberOption::error(const std::string& value,
                                const OptionContext& ctx) const
{
  const std::vector<std::string> pieces = NetworkSection::split(value);
  const Range bounds = range(ctx);
  std::vector<double> values;

  if (!mlist && pieces.size() != 1U) {
    return "A single value, not a list";
  }

  for (const std::string& piece : pieces) {
    std::istringstream text{piece};
    double number = 0.0;

    // the decimal point of no locale, the one of the darknet cfg
    text.imbue(std::locale::classic());

    if (mintegers) {
      long integer = 0;

      text >> integer;
      number = static_cast<double>(integer);
    } else {
      text >> number;
    }

    if (piece.empty() || text.fail() || !text.eof() || !std::isfinite(number)) {
      return "The '" + piece + "' isn't " +
             (mintegers ? "an integer" : "a real number");
    }

    if (number < bounds.first || number > bounds.second) {
      return "The " + piece + " isn't " +
             (bounds.first > bounds.second ? "possible" : range_text(bounds));
    }

    values.push_back(number);
  }

  return check(values, ctx);
}

std::string NumberOption::text_of(const double value)
{
  static constexpr const double INTEGERS = 1e15;
  static constexpr const int DIGITS = 10;

  if (std::trunc(value) == value && std::fabs(value) < INTEGERS) {
    return std::to_string(static_cast<long long>(value));
  }

  std::ostringstream text;

  text.imbue(std::locale::classic());
  text << std::setprecision(DIGITS) << value;

  return text.str();
}

NumberOption::Range NumberOption::range(const OptionContext& ctx) const
{
  (void)ctx;
  return mrange;
}

std::string NumberOption::rule(const OptionContext& ctx) const
{
  (void)ctx;
  return {};
}

std::string NumberOption::check(const std::vector<double>& values,
                                const OptionContext& ctx) const
{
  (void)values;
  (void)ctx;
  return {};
}

std::string NumberOption::range_text(const Range& bounds)
{
  if (bounds.first > bounds.second) {
    return "of no possible value";
  }

  const bool lowest = bounds.first > -NONE;
  const bool highest = bounds.second < NONE;

  if (lowest && highest) {
    return "from " + text_of(bounds.first) + " to " + text_of(bounds.second);
  }

  if (lowest || highest) {
    return lowest ? text_of(bounds.first) + " or more"
                  : text_of(bounds.second) + " or less";
  }

  return {};
}

}  // namespace net_editor
