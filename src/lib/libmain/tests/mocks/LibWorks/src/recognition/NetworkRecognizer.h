#ifndef DARKNETXX_PROJECT_NETWORKRECOGNIZER_CLASS_H
#define DARKNETXX_PROJECT_NETWORKRECOGNIZER_CLASS_H

#include "IRecognizer.h"

namespace darknetxx_impl
{

class NetworkRecognizer : public darknetxx011::IRecognizer
{
 public:
  darknetxx011::Recognition recognize(
      const darknetxx011::LibraryContext&) override
  {
    return {};
  }
};

}  // namespace darknetxx_impl

#endif  // DARKNETXX_PROJECT_NETWORKRECOGNIZER_CLASS_H
