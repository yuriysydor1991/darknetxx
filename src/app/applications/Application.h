#ifndef YOUR_CPP_APP_TEMPLATE_PROJECT_APPLICATION_CLASS_H
#define YOUR_CPP_APP_TEMPLATE_PROJECT_APPLICATION_CLASS_H

#include <memory>

#include "LibraryContext.h"
#include "src/app/ApplicationContext.h"
#include "src/app/applications/IApplication.h"
#include "src/converters/ApplicationContext2LibraryContext.h"

namespace app
{

/**
 * @brief The default application implementation should be placed into
 * into current class.
 *
 * The OS signals are not handled here, since the app::SignalsHandler component
 * covers them for every application of the project - the ApplicationFactory
 * installs it before the current class' run method is reached, so the only
 * thing left is to poll the raised ApplicationContext flags. See the
 * [handling the OS signals](/doc/sections/en_US/4-8-handling-the-OS-signals.md)
 * documentation section.
 */
class Application : public IApplication
{
 public:
  using LibraryContextPtr = darknetxx011::LibraryContextPtr;
  using CtxConverterPtr = converters::ApplicationContext2LibraryContextPtr;

  virtual ~Application() = default;
  Application(const Application&) = delete;
  Application(Application&&) = delete;
  Application();

  /**
   * @brief Implemented default application. See IApplication interface.
   *
   * @param ctx Application's run context with command line parameters etc.
   *
   * @return Returns the application return status. A zero value on the success,
   * the 128 + signal number on a stop request of the signal and other value
   * otherwise.
   */
  virtual int run(AppCtxPtr ctx) override;

 protected:
  /**
   * @brief The work of the prepared application: the library work the
   * context names, the training, the detection or the network weights file
   * conversion.
   *
   * @return Returns the status of the run method.
   */
  virtual int perform(AppCtxPtr ctx);

  /// @brief Draws the training chart of the weights file into the PNG one
  /// (see the ApplicationContext::draw_chart), no library work.
  virtual int draw_chart(AppCtxPtr ctx);

  /// @brief The library context of the application one, stopped by it's stop
  /// flag.
  virtual LibraryContextPtr create_and_convert_libctx(AppCtxPtr ctx);

  virtual CtxConverterPtr create_ctx_converter();

  /**
   * @brief Provides the exit status of the work the stop request of the
   * ApplicationContext::stop_signal cut short.
   */
  static int stopped_status(AppCtxPtr ctx);

  /**
   * @brief Makes the invalid computations noticeable instead of letting them
   * spread through the network as a NaN value.
   *
   * Enables the floating point exceptions and traps the SIGFPE signal they
   * arrive as. The trap belongs here and not to the app::SignalsHandler
   * component, since it is a diagnostic of a dying process and not an
   * application lifecycle request the component covers.
   *
   * The Darknet computations rely on the IEEE semantics of the overflows and
   * the invalid operations here and there (the logistic activation of a
   * confident logit overflows the expf, for example), so the method does
   * nothing at all unless the ENABLE_FP_TRAPS CMake option is ON.
   */
  static void init_computation_traps();

  AppCtxPtr actx;
};

}  // namespace app

#endif  // YOUR_CPP_APP_TEMPLATE_PROJECT_APPLICATION_CLASS_H
