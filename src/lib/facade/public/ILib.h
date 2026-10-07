#ifndef DARKNETXX_011_ILIB_CLASS_H
#define DARKNETXX_011_ILIB_CLASS_H

#include <memory>

#include "LibraryAPI.h"
#include "LibraryContext.h"

namespace darknetxx011
{

/**
 * @brief A work of the library: the training, the detection or the network
 * weights files conversion one.
 */
class DARKNETXX_LIB_API ILib
{
 public:
  using ILibPtr = std::shared_ptr<ILib>;

  virtual ~ILib() = default;
  ILib() = default;

  /// @brief Performs the work the context names, false on a failure.
  virtual bool libcall(LibraryContextPtr ctx) = 0;
};

using ILibPtr = ILib::ILibPtr;

}  // namespace darknetxx011

#endif  // DARKNETXX_011_ILIB_CLASS_H
