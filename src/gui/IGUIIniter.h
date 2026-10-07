#ifndef DARKNETXX_PROJECT_GUI_IGUIINITER_ABSTRACT_CLASS_H
#define DARKNETXX_PROJECT_GUI_IGUIINITER_ABSTRACT_CLASS_H

#include <memory>

#include "LibraryContext.h"
#include "src/app/ApplicationContext.h"

/**
 * @brief The namespace of the graphical user interface entities.
 */
namespace gui
{

/**
 * @brief The toplevel GUI of a toolkit.
 */
class IGUIIniter
{
 public:
  virtual ~IGUIIniter() = default;
  IGUIIniter() = default;

  /**
   * @brief Shows the window and runs the toolkit main loop till the user
   * closes the window or the application stop flag is raised.
   *
   * @param libctx The context of the library trainings of the window.
   *
   * @return Returns false on an error.
   */
  virtual bool run(app::AppCtxPtr ctx,
                   darknetxx011::LibraryContextPtr libctx) = 0;

  /// @brief Releases whatever the run() call acquired.
  virtual void deinit() = 0;
};

}  // namespace gui

#endif  // DARKNETXX_PROJECT_GUI_IGUIINITER_ABSTRACT_CLASS_H
