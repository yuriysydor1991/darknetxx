#ifndef DARKNETXX_PROJECT_NETWORKRECOGNIZER_CLASS_H
#define DARKNETXX_PROJECT_NETWORKRECOGNIZER_CLASS_H

#include "IRecognizer.h"

namespace darknetxx_impl
{

class NetworkRecognizer : public darknetxx012::IRecognizer
{
 public:
  darknetxx012::Recognition recognize(
      const darknetxx012::LibraryContext&) override
  {
    return {};
  }
};

}  // namespace darknetxx_impl

#endif  // DARKNETXX_PROJECT_NETWORKRECOGNIZER_CLASS_H
