#ifndef DARKNETXX_PROJECT_GUI_NET_EDITOR_NETWORKPROJECT_CLASS_H
#define DARKNETXX_PROJECT_GUI_NET_EDITOR_NETWORKPROJECT_CLASS_H

#include <string>

#include "src/gui/net-editor/NetworkModel.h"

namespace net_editor
{

/**
 * @brief The network file the editor edits: the network of the dxxwjz3p
 * project (or of another file of the network, see the
 * LibraryFacade::network_description()), or the new network of no file yet.
 */
class NetworkProject
{
 public:
  /// @brief The file name extension of the projects the editor saves.
  inline static const std::string EXTENSION{".dxxwjz3p"};

  virtual ~NetworkProject() = default;

  /// @brief The new network.
  NetworkProject();

  /// @brief Starts the new network of no file: the [net] section of the
  /// common training options, no layers and no class names.
  void create();

  /**
   * @brief Opens the network of the file.
   *
   * @param message Out: the reason of the failure.
   *
   * @return False for the file of no network, which leaves the project as it
   * was.
   */
  bool open(const std::string& path, std::string& message);

  /**
   * @brief Saves the network into the file, the project of it then: the
   * annotations and the training of the file of the project go on, so do
   * it's weights the network keeps (see the LibraryFacade::save_network()).
   *
   * @param message Out: the reason of the failure (the wrong options of the
   * network among them), or the weights the save dropped.
   *
   * @return False on any failure, which leaves the file as it was.
   */
  bool save(const std::string& path, std::string& message);

  /// @brief The project file of the path the user chose: the dxxwjz3p one
  /// of the path of no file name extension.
  static std::string project_of(const std::string& chosen);

  /// @brief The file of the project, empty for the new network.
  const std::string& path() const;

  /// @brief Tells the network changed since it was opened or saved.
  bool modified() const;

  /// @brief The name of the file, of the "*" of the modified network.
  std::string title() const;

  NetworkModel model;

 private:
  /// @brief The network and the class names of the moment.
  std::string snapshot() const;

  std::string mpath;
  std::string msaved;
};

}  // namespace net_editor

#endif  // DARKNETXX_PROJECT_GUI_NET_EDITOR_NETWORKPROJECT_CLASS_H
