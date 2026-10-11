#ifndef DARKNETXX_PROJECT_OMPCONTROLLER_CLASS_H
#define DARKNETXX_PROJECT_OMPCONTROLLER_CLASS_H

#include <functional>
#include <memory>

#include "LibraryContext.h"

namespace ompi
{

/**
 * @brief Test fake for the OMPController used by the app unit tests. By default
 * adjust()/adjust_threads() succeed; a test may install on_adjust to control
 * the result or to capture the passed context.
 */
class OMPController
{
 public:
  using OMPControllerPtr = std::shared_ptr<OMPController>;
  using LibCtxPtr = darknetxx011::LibraryContextPtr;

  virtual ~OMPController() = default;
  OMPController() = default;

  static OMPControllerPtr create();

  /// @brief Test seam to control adjust(). When unset, adjust() returns true.
  inline static std::function<bool(LibCtxPtr)> on_adjust;

  virtual bool adjust(LibCtxPtr ctx)
  {
    return on_adjust ? on_adjust(ctx) : true;
  }

  virtual bool adjust_threads(const unsigned int&) { return true; }
};

using OMPControllerPtr = OMPController::OMPControllerPtr;

}  // namespace ompi

#endif  // DARKNETXX_PROJECT_OMPCONTROLLER_CLASS_H
