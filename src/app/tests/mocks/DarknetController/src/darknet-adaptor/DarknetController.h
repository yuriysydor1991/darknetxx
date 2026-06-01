#ifndef DARKNET_CXX_PORT_PROJECT_APPLICATION_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_APPLICATION_CLASS_H

#include <functional>
#include <memory>

#include "src/app/ApplicationContext.h"

namespace darknet_adaptor
{

/**
 * @brief Test fake for the DarknetController. It avoids pulling the whole
 * original Darknet stack into the app unit tests. By default perform()
 * succeeds; a test may install on_perform to control the result or to capture
 * the passed context.
 */
class DarknetController
{
 public:
  using AppCtxPtr = app::AppCtxPtr;
  using DarknetControllerPtr = std::shared_ptr<DarknetController>;

  virtual ~DarknetController() = default;
  DarknetController() = default;

  static DarknetControllerPtr create();

  /// @brief Test seam to control perform(). When unset, perform() returns true.
  inline static std::function<bool(AppCtxPtr)> on_perform;

  virtual bool perform(AppCtxPtr ctx)
  {
    return on_perform ? on_perform(ctx) : true;
  }
};

using DarknetControllerPtr = DarknetController::DarknetControllerPtr;

}  // namespace darknet_adaptor

#endif  // DARKNET_CXX_PORT_PROJECT_APPLICATION_CLASS_H
