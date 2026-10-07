#ifndef DARKNETXX_PROJECT_APPLICATIONCONTEXT2LIBRARYCONTEXT_CLASS_H
#define DARKNETXX_PROJECT_APPLICATIONCONTEXT2LIBRARYCONTEXT_CLASS_H

#include <gmock/gmock.h>

#include <functional>
#include <memory>

#include "LibraryContext.h"
#include "src/app/ApplicationContext.h"

namespace converters
{

class ApplicationContext2LibraryContext
{
 public:
  using LibraryContextPtr = darknetxx011::LibraryContextPtr;

  virtual ~ApplicationContext2LibraryContext() = default;
  ApplicationContext2LibraryContext()
  {
    ON_CALL(*this, convert).WillByDefault(testing::Return(true));

    if (onMockCreate) {
      onMockCreate(*this);
    }
  }

  inline static std::function<void(ApplicationContext2LibraryContext&)>
      onMockCreate;

  MOCK_METHOD(bool, convert, (app::AppCtxPtr appctx, LibraryContextPtr libctx));
};

using ApplicationContext2LibraryContextPtr =
    std::shared_ptr<ApplicationContext2LibraryContext>;

}  // namespace converters

#endif  // DARKNETXX_PROJECT_APPLICATIONCONTEXT2LIBRARYCONTEXT_CLASS_H
