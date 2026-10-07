#ifndef DARKNETXX_PROJECT_GUI_GTKMM4_MAINWINDOWCONTEXT_CLASS_H
#define DARKNETXX_PROJECT_GUI_GTKMM4_MAINWINDOWCONTEXT_CLASS_H

#include <memory>

#include "ITrainer.h"
#include "LibraryContext.h"
#include "src/app/ApplicationContext.h"
#include "src/gui/gtkmm4/main-window/WindowEventsHandler.h"
#include "src/gui/gtkmm4/main-window/WindowLoader.h"

/**
 * @brief The namespace of the GTKmm 4 GUI.
 */
namespace darknetxxGtkmm4
{

/**
 * @brief The parts of the window the event handlers share.
 */
class MainWindowContext
{
 public:
  virtual ~MainWindowContext() = default;
  MainWindowContext() = default;

  static std::shared_ptr<MainWindowContext> build_context(
      app::AppCtxPtr nactx, darknetxx011::LibraryContextPtr nlibctx);

  /// @brief Waits for the stopped training and breaks the references of the
  /// parts to each other.
  virtual void clear();

  app::AppCtxPtr actx;
  darknetxx011::LibraryContextPtr libctx;
  std::shared_ptr<window::WindowLoader> wloader;
  std::shared_ptr<window::WindowEventsHandler> weHandler;
  darknetxx011::ITrainerPtr trainer;
};

}  // namespace darknetxxGtkmm4

#endif  // DARKNETXX_PROJECT_GUI_GTKMM4_MAINWINDOWCONTEXT_CLASS_H
