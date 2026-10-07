#ifndef DARKNETXX_PROJECT_ILIBMOCK_CLASS_H
#define DARKNETXX_PROJECT_ILIBMOCK_CLASS_H

#include <gmock/gmock.h>

#include "ILib.h"

namespace darknetxx011
{

class ILibMock : public ILib
{
 public:
  MOCK_METHOD(bool, libcall, (LibraryContextPtr ctx), (override));
};

}  // namespace darknetxx011

#endif  // DARKNETXX_PROJECT_ILIBMOCK_CLASS_H
