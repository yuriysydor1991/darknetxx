#ifndef DARKNETXX_PROJECT_OMPCONTROLLER_CLASS_H
#define DARKNETXX_PROJECT_OMPCONTROLLER_CLASS_H

#include <memory>

#include "src/app/ApplicationContext.h"

/**
 * @brief The OpenMP controllers component namespace.
 */
namespace ompi
{

/**
 * @brief The OpenMP controller class.
 */
class OMPController
{
 public:
  using OMPControllerPtr = std::shared_ptr<OMPController>;
  using AppCtxPtr = app::AppCtxPtr;

  virtual ~OMPController() = default;
  OMPController() = default;

  virtual bool adjust(AppCtxPtr ctx);

  virtual bool adjust_threads(const unsigned int& maxThreads);

  static OMPControllerPtr create();
};

using OMPControllerPtr = OMPController::OMPControllerPtr;

}  // namespace ompi

#endif  // DARKNETXX_PROJECT_OMPCONTROLLER_CLASS_H
