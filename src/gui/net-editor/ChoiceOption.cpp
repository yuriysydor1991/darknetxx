#include "src/gui/net-editor/ChoiceOption.h"

#include <algorithm>

namespace net_editor
{

ChoiceOption::ChoiceOption(std::string nkey, std::string ndef,
                           std::string nabout,
                           std::vector<std::string> nchoices)
    : AOption{std::move(nkey), std::move(ndef), std::move(nabout)},
      mchoices{std::move(nchoices)}
{
}

std::vector<std::string> ChoiceOption::choices() const { return mchoices; }

std::string ChoiceOption::limits(const OptionContext& ctx) const
{
  static constexpr const std::size_t LISTED = 6U;

  (void)ctx;

  if (mchoices.size() > LISTED) {
    return "one of the " + std::to_string(mchoices.size()) +
           " values of the list";
  }

  std::string text = "one of the";

  for (const auto& choice : mchoices) {
    text += (&choice == &mchoices.front() ? " " : ", ") + choice;
  }

  return text;
}

std::string ChoiceOption::error(const std::string& value,
                                const OptionContext& ctx) const
{
  return std::find(mchoices.begin(), mchoices.end(), value) == mchoices.end()
             ? "The value isn't " + limits(ctx)
             : std::string{};
}

}  // namespace net_editor
