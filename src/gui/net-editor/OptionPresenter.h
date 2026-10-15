#ifndef DARKNETXX_PROJECT_GUI_NET_EDITOR_OPTIONPRESENTER_CLASS_H
#define DARKNETXX_PROJECT_GUI_NET_EDITOR_OPTIONPRESENTER_CLASS_H

#include <memory>
#include <string>
#include <vector>

#include "src/gui/net-editor/AOption.h"
#include "src/gui/net-editor/NetworkModel.h"
#include "src/gui/net-editor/SectionSchema.h"

namespace net_editor
{

/**
 * @brief An option of a section the way the GUIs of every toolkit show it, a
 * row of the key, the editor of the value and the description of it, which
 * tells the values the option takes and the reason the value is wrong.
 */
class OptionPresenter
{
 public:
  using OptionPresenterPtr = std::shared_ptr<OptionPresenter>;
  using OptionPresenters = std::vector<OptionPresenterPtr>;

  OptionPresenter(const NetworkModel& nmodel, NetworkSectionPtr nsection,
                  std::string nkey);

  /// @brief The rows of the section: the options of it's schema, then the
  /// rest of the options it has.
  static OptionPresenters of(const NetworkModel& model,
                             const NetworkSectionPtr& section);

  /// @brief The schema of the section of the model.
  static const SectionSchema& schema_of(const NetworkModel& model,
                                        const NetworkSection& section);

  const std::string& key() const;

  /// @brief The value of the section, empty for none.
  std::string value() const;

  /// @brief Sets the value of the section, the empty one removes the option.
  void value(const std::string& nvalue);

  /// @brief The values the option takes, none for the typed ones.
  std::vector<std::string> choices() const;

  /// @brief Tells the text typed so far may become the value.
  bool typable(const std::string& text) const;

  /// @brief The hint of no value: the default the darknet takes, the
  /// required one.
  std::string placeholder() const;

  /// @brief What the option does and the values it takes now.
  std::string description() const;

  /// @brief The reason the value is wrong, empty for none.
  std::string error() const;

 private:
  const NetworkModel& model;
  const NetworkSectionPtr section;
  const std::string mkey;
  const AOptionPtr option;
};

using OptionPresenterPtr = OptionPresenter::OptionPresenterPtr;

}  // namespace net_editor

#endif  // DARKNETXX_PROJECT_GUI_NET_EDITOR_OPTIONPRESENTER_CLASS_H
