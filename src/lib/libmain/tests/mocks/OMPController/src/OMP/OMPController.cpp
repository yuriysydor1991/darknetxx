#include "src/OMP/OMPController.h"

#include <memory>

namespace ompi
{

OMPController::OMPControllerPtr OMPController::create()
{
  return std::make_shared<OMPController>();
}

}  // namespace ompi
