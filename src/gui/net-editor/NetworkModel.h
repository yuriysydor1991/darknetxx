#ifndef DARKNETXX_PROJECT_GUI_NET_EDITOR_NETWORKMODEL_CLASS_H
#define DARKNETXX_PROJECT_GUI_NET_EDITOR_NETWORKMODEL_CLASS_H

#include <string>
#include <vector>

#include "src/gui/net-editor/NetworkSection.h"

namespace net_editor
{

/**
 * @brief The network the editor edits: the [net] section, the layers in
 * their order and the names of the classes the network detects.
 *
 * The sections are shared, so the views of a section keep it whatever the
 * layers before it become.
 */
class NetworkModel
{
 public:
  virtual ~NetworkModel() = default;
  NetworkModel();

  /**
   * @brief Reads the cfg text the darknet way: the white space of the lines
   * dropped, the empty and the comment lines skipped, the line of no '=' the
   * key of the empty value.
   *
   * @return False for the text of no [net] or [network] section first, which
   * leaves the model as it was.
   */
  bool read(const std::string& cfg);

  /// @brief The cfg text: the sections separated by the empty lines.
  std::string cfg() const;

  const NetworkSectionPtr& net() const;
  const std::vector<NetworkSectionPtr>& layers() const;

  /// @brief The index of the layer of the section, -1 for the [net] one and
  /// for none.
  int index_of(const NetworkSection& section) const;

  /// @brief The layer after the one of the index, nullptr for none.
  const NetworkSection* next(int index) const;

  /// @brief Appends the layer of the type and of the options.
  NetworkSectionPtr add(const std::string& type,
                        NetworkSection::Options options);

  /// @brief Removes the layer, the references to the later layers follow
  /// them.
  void remove(int index);

  /**
   * @brief Moves the layer to the index, the layers between them shift to
   * the place it left. The references to the moved layers follow them.
   */
  void move(int from, int to);

  /// @brief The names of the classes, the name of a class at the class
  /// index.
  std::vector<std::string> classes;

 private:
  /// @brief Takes the new order of the layers, the references remapped.
  void reorder(std::vector<NetworkSectionPtr> nlayers);

  NetworkSectionPtr mnet;
  std::vector<NetworkSectionPtr> mlayers;
};

}  // namespace net_editor

#endif  // DARKNETXX_PROJECT_GUI_NET_EDITOR_NETWORKMODEL_CLASS_H
