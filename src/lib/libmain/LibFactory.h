#ifndef DARKNETXX_PROJECT_LIBFACTORY_CLASS_H
#define DARKNETXX_PROJECT_LIBFACTORY_CLASS_H

#include <memory>

#include "ILib.h"
#include "IRecognizer.h"
#include "ITrainer.h"
#include "LibraryContext.h"

namespace darknetxx_impl
{

/**
 * @brief Creates the library works the facade gives out.
 */
class LibFactory
{
 public:
  using LibFactoryPtr = std::shared_ptr<LibFactory>;
  using ILibPtr = darknetxx012::ILibPtr;
  using ITrainerPtr = darknetxx012::ITrainerPtr;
  using IRecognizerPtr = darknetxx012::IRecognizerPtr;
  using LibraryContextPtr = darknetxx012::LibraryContextPtr;

  virtual ~LibFactory() = default;

  /// @brief Applies the default logging settings unless a logger is adopted.
  LibFactory();

  virtual LibraryContextPtr create_default_context();

  /// @brief See the darknetxx012::LibraryFacade::create_library.
  virtual ILibPtr create_library(LibraryContextPtr ctx);

  virtual ITrainerPtr create_trainer(LibraryContextPtr ctx);

  virtual IRecognizerPtr create_recognizer();

  static LibFactoryPtr create_factory();

 protected:
  virtual ILibPtr create_conversion_lib();

  /// @brief The training and the detection one.
  virtual ILibPtr create_darknet_lib();
};

using LibFactoryPtr = LibFactory::LibFactoryPtr;

}  // namespace darknetxx_impl

#endif  // DARKNETXX_PROJECT_LIBFACTORY_CLASS_H
