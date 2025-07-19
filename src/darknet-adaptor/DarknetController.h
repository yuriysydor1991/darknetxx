#ifndef DARKNET_CXX_PORT_PROJECT_APPLICATION_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_APPLICATION_CLASS_H

#include <memory>

#include "src/app/ApplicationContext.h"
#include "src/darknet-adaptor/DarknetContext.h"
#include "src/darknet-adaptor/adaptors/IDarknetAdaptor.h"

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
  using IDarknetAdaptorPtr = adaptors::IDarknetAdaptorPtr;

  virtual ~DarknetController() = default;
  DarknetController() = default;

  static DarknetControllerPtr create();

  /**
   * @brief Perform darkent action.
   *
   * @param ctx Application's run context with command line parameters etc.
   *
   * @return Returns true on the success and false otherwise.
   */
  virtual bool perform(AppCtxPtr ctx);

protected:
  virtual DarknetContextPtr create_context(AppCtxPtr actx);
  virtual IDarknetAdaptorPtr create_appropriate_worker(DarknetContextPtr dctx);
};

using DarknetControllerPtr = DarknetController::DarknetControllerPtr;

}  // namespace darknet_adaptor

#endif  // DARKNET_CXX_PORT_PROJECT_APPLICATION_CLASS_H
