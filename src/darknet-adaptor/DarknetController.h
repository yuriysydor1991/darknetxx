#ifndef DARKNET_CXX_PORT_PROJECT_APPLICATION_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_APPLICATION_CLASS_H

#include <memory>

#include "src/app/ApplicationContext.h"

namespace darknet_adaptor
{

/**
 * @brief The adaptor class for the orig Darknet code.
 */
class DarknetController
{
 public:
  using AppCtxPtr = app::AppCtxPtr;
  using DarknetControllerPtr = std::shared_ptr<DarknetController>;

  virtual ~DarknetController() = default;
  DarknetController() = default;

  static DarknetControllerPtr create();

  /**
   * @brief Init the darknet.
   *
   * @param ctx Application's run context with command line parameters etc.
   *
   * @return Returns true on the success and false otherwise.
   */
  virtual bool init(AppCtxPtr ctx);
};

using DarknetControllerPtr = DarknetController::DarknetControllerPtr;

}  // namespace darknet_adaptor

#endif  // DARKNET_CXX_PORT_PROJECT_APPLICATION_CLASS_H
