#ifndef DARKNETXX_PROJECT_LIBMAIN_CLASS_H
#define DARKNETXX_PROJECT_LIBMAIN_CLASS_H

#include "ILib.h"

namespace darknetxx_impl
{

class LibMain : public darknetxx012::ILib
{
 public:
  bool libcall(darknetxx012::LibraryContextPtr) override { return true; }
};

}  // namespace darknetxx_impl

#endif  // DARKNETXX_PROJECT_LIBMAIN_CLASS_H
