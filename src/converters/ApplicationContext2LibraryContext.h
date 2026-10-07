#ifndef DARKNETXX_PROJECT_APPLICATIONCONTEXT2LIBRARYCONTEXT_CLASS_H
#define DARKNETXX_PROJECT_APPLICATIONCONTEXT2LIBRARYCONTEXT_CLASS_H

#include <memory>

#include "LibraryContext.h"
#include "src/app/ApplicationContext.h"

/**
 * @brief The namespace of the conversions between the application and the
 * library entities.
 */
namespace converters
{

/**
 * @brief Transfers the command line of the application context into the
 * library context, which the stop flag of the application context stops.
 */
class ApplicationContext2LibraryContext
{
 public:
  using LibraryContextPtr = darknetxx011::LibraryContextPtr;

  virtual ~ApplicationContext2LibraryContext() = default;
  ApplicationContext2LibraryContext() = default;

  /// @brief False for a missing context.
  virtual bool convert(app::AppCtxPtr appctx, LibraryContextPtr libctx);

 protected:
  static void convert_darknet_params(const app::ApplicationContext& appctx,
                                     darknetxx011::LibraryContext& libctx);
  static void convert_conversion_params(const app::ApplicationContext& appctx,
                                        darknetxx011::LibraryContext& libctx);
};

using ApplicationContext2LibraryContextPtr =
    std::shared_ptr<ApplicationContext2LibraryContext>;

}  // namespace converters

#endif  // DARKNETXX_PROJECT_APPLICATIONCONTEXT2LIBRARYCONTEXT_CLASS_H
