#ifndef DARKNET_CXX_PORT_PROJECT_DARKNETCONTEXT_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_DARKNETCONTEXT_CLASS_H

#include <memory>

#include "src/app/ApplicationContext.h"
#include "src/darknet-adaptor/DarknetStrings.h"

namespace darknet_adaptor
{

/**
 * @brief The adaptors data context class.
 */
class DarknetContext
{
 public:
  using AppCtxPtr = app::AppCtxPtr;
  using DarknetContextPtr = std::shared_ptr<DarknetContext>;

  virtual ~DarknetContext() = default;
  explicit DarknetContext(AppCtxPtr nactx);

  // the app context
  AppCtxPtr actx;

  // command passed to the application binary
  std::string worker{DarknetStrings::detector};
  // a subcommand passed after the main command
  std::string command;
};

using DarknetContextPtr = DarknetContext::DarknetContextPtr;

}  // namespace darknet_adaptor

#endif  // DARKNET_CXX_PORT_PROJECT_DARKNETCONTEXT_CLASS_H
