#ifndef DARKNETXX_PROJECT_GUI_GTKMM4_RECOGNITIONEVENTSHANDLER_CLASS_H
#define DARKNETXX_PROJECT_GUI_GTKMM4_RECOGNITIONEVENTSHANDLER_CLASS_H

#include <glibmm/dispatcher.h>

#include <memory>
#include <thread>

#include "IRecognizer.h"
#include "src/gui/gtkmm4/IWindowEventHandler.h"

namespace darknetxxGtkmm4
{
class RecognitionWindowContext;
}  // namespace darknetxxGtkmm4

namespace darknetxxGtkmm4::window::event_handlers
{

/**
 * @brief The recognition of the image of the image entry by the network of
 * the weights, the cfg and the names entries, prefilled with the command line
 * files, which the Recognize button, the activated entries and the changed
 * threshold start. It runs in a thread of it's own, so the window stays
 * responsive, and shows the image with the objects boxed and their list.
 */
class RecognitionEventsHandler : public IWindowEventHandler
{
 public:
  /// @brief Waits for the running recognition.
  virtual ~RecognitionEventsHandler();
  explicit RecognitionEventsHandler(
      std::shared_ptr<RecognitionWindowContext> nmwctx);

  virtual bool subscribe() override;

 protected:
  /// @brief Recognizes the image of the entries, after the running
  /// recognition.
  virtual void recognize();
  virtual void show_recognition();

 private:
  std::shared_ptr<RecognitionWindowContext> mwctx;
  darknetxx011::IRecognizerPtr recognizer;

  /// @brief The running recognition, joined once it is shown.
  std::thread mworker;
  darknetxx011::Recognition mrecognition;
  Glib::Dispatcher mrecognized;

  /// @brief The entries changed while the recognition ran.
  bool mpending{false};
};

}  // namespace darknetxxGtkmm4::window::event_handlers

#endif  // DARKNETXX_PROJECT_GUI_GTKMM4_RECOGNITIONEVENTSHANDLER_CLASS_H
