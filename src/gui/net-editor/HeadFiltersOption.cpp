#include "src/gui/net-editor/HeadFiltersOption.h"

namespace net_editor
{

HeadFiltersOption::HeadFiltersOption(std::string nabout)
    : NumberOption{"filters", "1", std::move(nabout), true, false, {1.0, NONE}}
{
}

std::string HeadFiltersOption::rule(const OptionContext& ctx) const
{
  std::string formula;
  const int filters = head(ctx, formula);

  return filters < 0 ? std::string{}
                     : "the " + formula + " = " + std::to_string(filters);
}

std::string HeadFiltersOption::check(const std::vector<double>& values,
                                     const OptionContext& ctx) const
{
  std::string formula;
  const int filters = head(ctx, formula);

  return filters < 0 || static_cast<int>(values.front()) == filters
             ? std::string{}
             : "The " + formula + " = " + std::to_string(filters) + " filters";
}

int HeadFiltersOption::head(const OptionContext& ctx, std::string& formula)
{
  static constexpr const int YOLO_VALUES = 5;
  static constexpr const int GAUSSIAN_YOLO_VALUES = 9;
  const NetworkSection* next = ctx.model.next(ctx.index());
  const std::string type = next == nullptr ? std::string{} : next->type();

  if (type != "yolo" && type != "Gaussian_yolo" && type != "region") {
    return -1;
  }

  const int classes = next->integer("classes", 20);
  const int num = next->integer("num", 1);

  if (type == "region") {
    const int coords = next->integer("coords", 4);

    formula = "next [region] layer takes num * (classes + coords + 1) = " +
              std::to_string(num) + " * (" + std::to_string(classes) + " + " +
              std::to_string(coords) + " + 1)";

    return num * (classes + coords + 1);
  }

  const int masks = next->find("mask") == nullptr
                        ? num
                        : static_cast<int>(next->count("mask"));
  const int values = type == "yolo" ? YOLO_VALUES : GAUSSIAN_YOLO_VALUES;

  formula = "next [" + type + "] layer takes (classes + " +
            std::to_string(values) + ") * masks = (" + std::to_string(classes) +
            " + " + std::to_string(values) + ") * " + std::to_string(masks);

  return (classes + values) * masks;
}

}  // namespace net_editor
