#ifndef DARKNETXX_PROJECT_WEIGHTSCONVERSIONLIB_CLASS_H
#define DARKNETXX_PROJECT_WEIGHTSCONVERSIONLIB_CLASS_H

#include "ILib.h"

namespace darknetxx_impl
{

class WeightsConversionLib : public darknetxx011::ILib
{
 public:
  bool libcall(darknetxx011::LibraryContextPtr) override { return true; }
};

}  // namespace darknetxx_impl

#endif  // DARKNETXX_PROJECT_WEIGHTSCONVERSIONLIB_CLASS_H
