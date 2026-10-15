#include "src/gui/net-editor/OptionPresenter.h"

#include <algorithm>

#include "src/gui/net-editor/SectionSchemas.h"

namespace net_editor
{

OptionPresenter::OptionPresenter(const NetworkModel& nmodel,
                                 NetworkSectionPtr nsection, std::string nkey)
    : model{nmodel},
      section{std::move(nsection)},
      mkey{std::move(nkey)},
      option{schema_of(model, *section).find(mkey)}
{
}

OptionPresenter::OptionPresenters OptionPresenter::of(
    const NetworkModel& model, const NetworkSectionPtr& section)
{
  const SectionSchema& schema = schema_of(model, *section);
  OptionPresenters rows;

  for (const auto& known : schema.options()) {
    rows.push_back(
        std::make_shared<OptionPresenter>(model, section, known->key()));
  }

  for (const auto& option : section->options()) {
    const std::string& key = option.first;
    const bool shown = std::any_of(
        rows.begin(), rows.end(),
        [&key](const OptionPresenterPtr& row) { return row->key() == key; });

    if (!shown) {
      rows.push_back(std::make_shared<OptionPresenter>(model, section, key));
    }
  }

  return rows;
}

const SectionSchema& OptionPresenter::schema_of(const NetworkModel& model,
                                                const NetworkSection& section)
{
  return &section == model.net().get() ? SectionSchemas::net()
                                       : SectionSchemas::of(section.type());
}

const std::string& OptionPresenter::key() const { return mkey; }

std::string OptionPresenter::value() const
{
  const std::string* found = section->find(mkey);

  return found == nullptr ? std::string{} : *found;
}

void OptionPresenter::value(const std::string& nvalue)
{
  section->set(mkey, nvalue);
}

std::vector<std::string> OptionPresenter::choices() const
{
  return option == nullptr ? std::vector<std::string>{} : option->choices();
}

bool OptionPresenter::typable(const std::string& text) const
{
  return option == nullptr || option->typable(text);
}

std::string OptionPresenter::placeholder() const
{
  if (option == nullptr) {
    return {};
  }

  if (option->required(OptionContext{model, *section})) {
    return "required";
  }

  return option->def().empty() ? "unset" : "default: " + option->def();
}

std::string OptionPresenter::description() const
{
  if (option == nullptr) {
    return "The option the editor knows nothing of, kept as it is";
  }

  const std::string limits = option->limits(OptionContext{model, *section});

  return option->about() + (limits.empty() ? "" : ": " + limits);
}

std::string OptionPresenter::error() const
{
  const std::string* found = section->find(mkey);
  const OptionContext ctx{model, *section};

  if (option == nullptr) {
    return {};
  }

  if (found == nullptr) {
    return option->required(ctx) ? "The option is required" : std::string{};
  }

  // the key alone takes the default
  return found->empty() ? std::string{} : option->error(*found, ctx);
}

}  // namespace net_editor
