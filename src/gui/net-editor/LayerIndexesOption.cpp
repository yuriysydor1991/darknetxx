#include "src/gui/net-editor/LayerIndexesOption.h"

namespace net_editor
{

LayerIndexesOption::LayerIndexesOption(std::string nkey, std::string nabout,
                                       const bool nlist)
    : NumberOption{std::move(nkey), "", std::move(nabout), true, nlist}
{
}

bool LayerIndexesOption::required(const OptionContext& ctx) const
{
  (void)ctx;
  return true;
}

NumberOption::Range LayerIndexesOption::range(const OptionContext& ctx) const
{
  const int index = ctx.index();

  return {-index, index - 1};
}

std::string LayerIndexesOption::rule(const OptionContext& ctx) const
{
  const int index = ctx.index();
  const auto& layers = ctx.model.layers();
  std::string targets;

  for (const int reference : ctx.section.integers(key())) {
    const int target = reference < 0 ? index + reference : reference;

    if (target >= 0 && target < index) {
      targets += (targets.empty() ? "" : ", ") + std::to_string(target) + " [" +
                 layers[static_cast<std::size_t>(target)]->type() + "]";
    }
  }

  return "the layers before it, counted back for the negative ones" +
         (targets.empty() ? std::string{} : ": the layers " + targets);
}

}  // namespace net_editor
