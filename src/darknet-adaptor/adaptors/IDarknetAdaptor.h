#ifndef DARKNET_CXX_PORT_PROJECT_IDARKNETADAPTOR_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_IDARKNETADAPTOR_CLASS_H

#include <memory>

#include "src/darknet-adaptor/DarknetContext.h"

namespace darknet_adaptor::adaptors
{

/**
 * @brief The particular Darknet adaptors abstract class to define interface.
 */
class IDarknetAdaptor
{
 public:
  using IDarknetAdaptorPtr = std::shared_ptr<IDarknetAdaptor>;

  virtual ~IDarknetAdaptor() = default;
  IDarknetAdaptor() = default;

  virtual bool perform(DarknetContextPtr dctx) = 0;
};

using IDarknetAdaptorPtr = IDarknetAdaptor::IDarknetAdaptorPtr;

}  // namespace darknet_adaptor::adaptors

#endif  // DARKNET_CXX_PORT_PROJECT_IDARKNETADAPTOR_CLASS_H
