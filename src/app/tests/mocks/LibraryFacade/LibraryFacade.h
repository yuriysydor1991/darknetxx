#ifndef DARKNETXX_011_LIBRARYFACADE_CLASS_H
#define DARKNETXX_011_LIBRARYFACADE_CLASS_H

#include <functional>
#include <memory>
#include <string>
#include <tuple>
#include <vector>

#include "ILib.h"
#include "ILogger.h"
#include "ITrainer.h"
#include "LibraryContext.h"

namespace darknetxx011
{

/**
 * @brief Test fake of the library facade: the on_* hooks control what it
 * gives out, none (the nullptr) by default.
 */
class LibraryFacade
{
 public:
  using FormatsDescriptions = std::vector<std::tuple<std::string, std::string>>;

  inline static std::function<ILibPtr(LibraryContextPtr)> on_create_library;
  inline static std::function<ITrainerPtr(LibraryContextPtr)> on_create_trainer;
  inline static std::function<void(const logger::ILoggerPtr&)> on_init_logger;
  inline static std::function<std::vector<unsigned char>(const std::string&)>
      on_training_chart;

  static LibraryContextPtr create_library_context()
  {
    return std::make_shared<LibraryContext>();
  }

  static ILibPtr create_library(LibraryContextPtr ctx)
  {
    return on_create_library ? on_create_library(ctx) : nullptr;
  }

  static ITrainerPtr create_trainer(LibraryContextPtr ctx)
  {
    return on_create_trainer ? on_create_trainer(ctx) : nullptr;
  }

  static FormatsDescriptions conversion_formats()
  {
    return {{"weights", "the weights format"},
            {"dxxwjz1", "the dxxwjz1 format"},
            {"dxxwjz2", "the dxxwjz2 format"}};
  }

  static std::vector<unsigned char> training_chart(const std::string& weights)
  {
    return on_training_chart ? on_training_chart(weights)
                             : std::vector<unsigned char>{};
  }

  static void init_logger(const logger::ILoggerPtr& appLogger)
  {
    if (on_init_logger) {
      on_init_logger(appLogger);
    }
  }

  static void reset()
  {
    on_create_library = nullptr;
    on_create_trainer = nullptr;
    on_init_logger = nullptr;
    on_training_chart = nullptr;
  }
};

}  // namespace darknetxx011

#endif  // DARKNETXX_011_LIBRARYFACADE_CLASS_H
