#ifndef DARKNET_CXX_PORT_PROJECT_ADETECTOR_ADAPTOR_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_ADETECTOR_ADAPTOR_CLASS_H

#include <memory>

#include "src/darknet-adaptor/adaptors/IDarknetAdaptor.h"

namespace darknet_adaptor::adaptors
{

/**
 * @brief The adaptor class for the orig Darknet code.
 */
class ADetector : virtual public IDarknetAdaptor
{
 public:
  using AppCtxPtr = app::AppCtxPtr;
  using ADetectorPtr = std::shared_ptr<ADetector>;

  virtual ~ADetector() = default;
  ADetector() = default;

  virtual bool perform(DarknetContextPtr dctx) override;
};

using ADetectorPtr = ADetector::ADetectorPtr;

}  // namespace darknet_adaptor::adaptors

#endif  // DARKNET_CXX_PORT_PROJECT_ADETECTOR_ADAPTOR_CLASS_H
