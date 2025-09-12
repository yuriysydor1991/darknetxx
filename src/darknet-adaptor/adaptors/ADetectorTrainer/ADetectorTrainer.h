#ifndef DARKNET_CXX_PORT_PROJECT_ADETECTORTRAINER_ADAPTOR_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_ADETECTORTRAINER_ADAPTOR_CLASS_H

#include <memory>

#include "src/darknet-adaptor/DarknetContext.h"
#include "src/darknet-adaptor/adaptors/IDarknetAdaptor.h"

namespace darknet_adaptor::adaptors
{

/**
 * @brief The adaptor class for the orig Darknet code.
 */
class ADetectorTrainer : virtual public IDarknetAdaptor
{
 public:
  using AppCtxPtr = app::AppCtxPtr;
  using ADetectorTrainerPtr = std::shared_ptr<ADetectorTrainer>;

  virtual ~ADetectorTrainer() = default;
  ADetectorTrainer() = default;

  virtual bool perform(DarknetContextPtr dctx) override;

  static ADetectorTrainerPtr create();
};

using ADetectorTrainerPtr = ADetectorTrainer::ADetectorTrainerPtr;

}  // namespace darknet_adaptor::adaptors

#endif  // DARKNET_CXX_PORT_PROJECT_ADETECTORTRAINER_ADAPTOR_CLASS_H
