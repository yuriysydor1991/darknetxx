#ifndef DARKNETXX_PROJECT_GUI_NET_EDITOR_NETWORKSECTION_CLASS_H
#define DARKNETXX_PROJECT_GUI_NET_EDITOR_NETWORKSECTION_CLASS_H

#include <memory>
#include <string>
#include <utility>
#include <vector>

/**
 * @brief The namespace of the network editor model the GUIs of every toolkit
 * share.
 */
namespace net_editor
{

/**
 * @brief A section of the darknet cfg: the [net] one or a layer, of it's type
 * (the name between the brackets) and the key=value options in their order.
 */
class NetworkSection
{
 public:
  using Option = std::pair<std::string, std::string>;
  using Options = std::vector<Option>;

  virtual ~NetworkSection() = default;
  explicit NetworkSection(std::string ntype, Options noptions = {});

  const std::string& type() const;
  const Options& options() const;

  /// @brief The value of the key, nullptr for none. The key of no '=' is the
  /// one of the empty value, the darknet NULL one.
  const std::string* find(const std::string& key) const;

  /// @brief Sets the value of the present key or adds the key after the
  /// others, the empty value removes the key.
  void set(const std::string& key, const std::string& value);

  /// @brief The value of the key the darknet atoi() way, the default for
  /// none.
  int integer(const std::string& key, int def) const;

  /// @brief The comma separated values of the key the darknet atoi() way,
  /// none for no key.
  std::vector<int> integers(const std::string& key) const;

  /// @brief The count of the comma separated values of the key, 0 for none.
  std::size_t count(const std::string& key) const;

  /// @brief The cfg text: the [type] line and a line of each option, the key
  /// alone of the empty value.
  std::string cfg() const;

  /// @brief The options of the cfg lines, separated by the spaces.
  std::string summary() const;

  /// @brief The comma separated values of the text.
  static std::vector<std::string> split(const std::string& text);

 private:
  std::string mtype;
  Options moptions;
};

using NetworkSectionPtr = std::shared_ptr<NetworkSection>;

}  // namespace net_editor

#endif  // DARKNETXX_PROJECT_GUI_NET_EDITOR_NETWORKSECTION_CLASS_H
