#include "src/gui/net-editor/InputSizeOption.h"

#include <algorithm>
#include <set>

namespace net_editor
{

InputSizeOption::InputSizeOption(std::string nkey, std::string nabout)
    : NumberOption{std::move(nkey), "",         std::move(nabout), true,
                   false,           {1.0, NONE}}
{
}

bool InputSizeOption::required(const OptionContext& ctx) const
{
  return ctx.section.find("inputs") == nullptr;
}

int InputSizeOption::stride(const NetworkModel& model)
{
  static const std::set<std::string> detections{"yolo", "Gaussian_yolo",
                                                "region", "detection"};
  static const std::set<std::string> downsampling{
      "convolutional", "conv",      "maxpool", "max",
      "local_avgpool", "local_avg", "reorg",   "reorg3d"};
  const auto& layers = model.layers();
  std::vector<int> factors;
  int factor = 1;
  int deepest = 1;
  bool detects = false;

  for (const auto& layer : layers) {
    const std::string& type = layer->type();
    const int index = static_cast<int>(factors.size());

    if (type == "route") {
      // the outputs of the first layer of the route go on
      const std::vector<int> refs = layer->integers("layers");
      const int first =
          refs.empty() ? -1 : (refs[0] < 0 ? index + refs[0] : refs[0]);

      factor = first >= 0 && first < index
                   ? factors[static_cast<std::size_t>(first)]
                   : factor;
    } else if (type == "upsample") {
      const int up = layer->integer("stride", 2);

      factor = up < 0 ? factor * -up : std::max(1, factor / std::max(1, up));
    } else if (downsampling.count(type) > 0U) {
      factor *= std::max(1, layer->integer("stride", 1));
    }

    detects = detects || detections.count(type) > 0U;
    deepest = std::max(deepest, factor);
    factors.push_back(factor);
  }

  return detects ? deepest : 1;
}

std::string InputSizeOption::rule(const OptionContext& ctx) const
{
  const int multiple = stride(ctx.model);

  return multiple > 1 ? "a multiple of the " + std::to_string(multiple) +
                            " the layers downsample it by"
                      : std::string{};
}

std::string InputSizeOption::check(const std::vector<double>& values,
                                   const OptionContext& ctx) const
{
  const int multiple = stride(ctx.model);

  return static_cast<long>(values.front()) % multiple == 0
             ? std::string{}
             : "The " + key() + " isn't a multiple of the " +
                   std::to_string(multiple) +
                   " the layers downsample it by for the detection layers";
}

}  // namespace net_editor
