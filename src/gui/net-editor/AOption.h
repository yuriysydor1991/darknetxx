#ifndef DARKNETXX_PROJECT_GUI_NET_EDITOR_AOPTION_CLASS_H
#define DARKNETXX_PROJECT_GUI_NET_EDITOR_AOPTION_CLASS_H

#include <memory>
#include <string>
#include <vector>

#include "src/gui/net-editor/OptionContext.h"

namespace net_editor
{

/**
 * @brief The option of a cfg section the editor knows: it's key, the value
 * the darknet takes for none, what it does and the values it takes, which may
 * depend on the other options of the network.
 */
class AOption
{
 public:
  virtual ~AOption() = default;

  /**
   * @param nkey The key of the cfg line.
   * @param ndef The value the darknet takes for the absent key, empty for
   * none (the option of no default, a required one or one of the computed
   * default).
   * @param nabout What the option does.
   */
  AOption(std::string nkey, std::string ndef, std::string nabout);

  const std::string& key() const;
  const std::string& def() const;
  const std::string& about() const;

  /// @brief The values the option takes, none for the typed ones.
  virtual std::vector<std::string> choices() const;

  /// @brief Tells the text typed so far may become the value (the input
  /// filter of the entry).
  virtual bool typable(const std::string& text) const;

  /// @brief The values the option takes in the context, empty for any.
  virtual std::string limits(const OptionContext& ctx) const = 0;

  /// @brief The reason the value is wrong in the context, empty for none.
  virtual std::string error(const std::string& value,
                            const OptionContext& ctx) const = 0;

  /// @brief Tells the section of the context needs the option.
  virtual bool required(const OptionContext& ctx) const;

 private:
  const std::string mkey;
  const std::string mdef;
  const std::string mabout;
};

using AOptionPtr = std::shared_ptr<const AOption>;

}  // namespace net_editor

#endif  // DARKNETXX_PROJECT_GUI_NET_EDITOR_AOPTION_CLASS_H
