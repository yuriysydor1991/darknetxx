#ifndef DARKNETXX_PROJECT_LIBMAIN_CLASS_H
#define DARKNETXX_PROJECT_LIBMAIN_CLASS_H

#include "ILib.h"
#include "LibraryContext.h"

namespace darknetxx_impl
{

/**
 * @brief The darknet work: the detection of the detect_cfg network or the
 * training of the train_cfg one.
 */
class LibMain : public darknetxx011::ILib
{
 public:
  using LibraryContextPtr = darknetxx011::LibraryContextPtr;

  virtual ~LibMain() = default;
  LibMain() = default;

  virtual bool libcall(LibraryContextPtr ctx) override;

 protected:
  /// @brief The OpenMP threads of the context.
  virtual bool adjust_threads(LibraryContextPtr ctx);
};

}  // namespace darknetxx_impl

#endif  // DARKNETXX_PROJECT_LIBMAIN_CLASS_H
