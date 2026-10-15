#ifndef DARKNETXX_PROJECT_GUI_GTKMM4_PROJECTEVENTSHANDLER_CLASS_H
#define DARKNETXX_PROJECT_GUI_GTKMM4_PROJECTEVENTSHANDLER_CLASS_H

#include <gtkmm/filechooserdialog.h>
#include <gtkmm/messagedialog.h>

#include <functional>
#include <memory>
#include <optional>
#include <string>

#include "src/gui/gtkmm4/IWindowEventHandler.h"

namespace darknetxxGtkmm4
{
class NetEditorWindowContext;
}  // namespace darknetxxGtkmm4

namespace darknetxxGtkmm4::window::event_handlers
{

/**
 * @brief The file of the network: the New, the Open…, the Save and the Save
 * as… buttons, the project of the --train-cfg command line parameter opened
 * at once, the title of the file and of it's changes. The network changed
 * since it's save is saved, discarded or kept on, the user chooses, before
 * the window closes, the new network starts or another one opens.
 */
class ProjectEventsHandler : public IWindowEventHandler
{
 public:
  /// @brief The responses of the prompt of the changed network.
  inline static constexpr const int SAVE = 1;
  inline static constexpr const int DISCARD = 2;

  virtual ~ProjectEventsHandler() = default;
  explicit ProjectEventsHandler(std::shared_ptr<NetEditorWindowContext> nmwctx);

  virtual bool subscribe() override;

  /// @brief The prompt of the changed network, nullptr before the first one.
  Gtk::MessageDialog* prompt();

  /// @brief The file chooser of the Save as…, nullptr before the first one.
  Gtk::FileChooserDialog* save_chooser();

  /// @brief Saves into the file, tells the outcome.
  virtual bool save_into(const std::string& path);

 protected:
  virtual void create();
  virtual void open();
  virtual void save();
  virtual void save_as();

  /**
   * @brief Runs the action once the changed network is saved or discarded,
   * the user chooses.
   *
   * @param name The name of the action of the prompt, "exit" for example.
   */
  virtual void confirm(const std::string& name, std::function<void()> action);

  virtual bool on_close_request();

 private:
  void on_prompted(int response);
  void on_open_chosen(int response);
  void on_save_chosen(int response);
  /// @brief Takes the title of the shown window.
  void on_shown();
  void show_title();
  void reload();

  std::shared_ptr<NetEditorWindowContext> mwctx;
  std::unique_ptr<Gtk::MessageDialog> mprompt;
  std::unique_ptr<Gtk::FileChooserDialog> mopen;
  std::unique_ptr<Gtk::FileChooserDialog> msave;
  /// the action of the prompt, which waits for the save as… file as well
  std::function<void()> maction;
  /// the window closes for the user chose it
  bool mclosing{false};
  /// the title of the shown window of no network, none before it's shown
  std::optional<Glib::ustring> mtitle;
};

}  // namespace darknetxxGtkmm4::window::event_handlers

#endif  // DARKNETXX_PROJECT_GUI_GTKMM4_PROJECTEVENTSHANDLER_CLASS_H
