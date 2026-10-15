#ifndef DARKNETXX_PROJECT_GUI_QT6_NETEDITORMAINWINDOW_CLASS_H
#define DARKNETXX_PROJECT_GUI_QT6_NETEDITORMAINWINDOW_CLASS_H

#include <QAction>
#include <QComboBox>
#include <QGridLayout>
#include <QLabel>
#include <QMainWindow>
#include <QPlainTextEdit>
#include <QToolButton>
#include <QVBoxLayout>
#include <memory>
#include <string>
#include <vector>

#include "src/gui/net-editor/NetworkProject.h"
#include "src/gui/qt6/LayerRowWidget.h"
#include "src/gui/qt6/QtOptionsGrid.h"

namespace darknetxxQt6
{

/**
 * @brief The window of the network editor: the New, the Open…, the Save and
 * the Save as… actions of the file of the network, the rows of the [net]
 * options, the class names and the rows of the layers, which the + button
 * adds the layer of the chosen type to, the - button of a row removes and
 * the drag of the handle of a row moves. Every edit refreshes the limits and
 * the wrong values of every row, since they depend on the rest of the
 * network. The network changed since it's save is saved, discarded or kept
 * on, the user chooses, before the window closes, the new network starts or
 * another one opens.
 */
class NetEditorMainWindow : public QMainWindow
{
 public:
  /// @param nname The name of the binary, the title of the window.
  /// @param file The file of the network to open, empty for the new one.
  NetEditorMainWindow(const std::string& nname, const std::string& file);

  /// @brief Opens the network of the file, tells the outcome.
  bool open_file(const std::string& path);

  /// @brief Saves the network into the file, tells the outcome.
  bool save_into(const std::string& path);

  /// @brief Adds the layer of the type at the bottom.
  void add_layer(const std::string& type);

  /// @brief Moves the layer to the index.
  void move_layer(int from, int to);

  net_editor::NetworkProject& project();
  QtOptionsGrid& net_rows();
  const std::vector<LayerRowWidget*>& rows() const;
  QPlainTextEdit& classes_edit();
  QComboBox& layer_type_combo();
  QToolButton& add_layer_button();
  QLabel& status_label();
  QAction& save_action();

 protected:
  virtual void closeEvent(QCloseEvent* event) override;

 private:
  void create();
  void open();
  void save();
  /// @brief Saves into the file the user chooses, false for none.
  bool save_as();

  /**
   * @brief Asks the user to save or to discard the changed network before
   * the action of the name, "exit" for example.
   *
   * @return False for the user canceling the action (or the failed save).
   */
  bool confirm(const std::string& name);

  void remove_layer(LayerRowWidget& row);
  LayerRowWidget* append(const net_editor::NetworkSectionPtr& section);
  void show_network();
  void changed();
  void show_title();
  void tell(const std::string& message, bool failure = false);

  const QString mname;
  net_editor::NetworkProject mproject;
  QLabel* mfile;
  QGridLayout* mnetGrid;
  QWidget* mnet;
  QPlainTextEdit* mclasses;
  QVBoxLayout* mlayers;
  QComboBox* mtype;
  QLabel* mtypeAbout;
  QToolButton* madd;
  QLabel* mstatus;
  QAction* msave;
  std::unique_ptr<QtOptionsGrid> mnetRows;
  std::vector<LayerRowWidget*> mrows;
  /// the class names shown, not edited
  bool mshowing{false};
};

}  // namespace darknetxxQt6

#endif  // DARKNETXX_PROJECT_GUI_QT6_NETEDITORMAINWINDOW_CLASS_H
