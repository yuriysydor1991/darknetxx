#ifndef DARKNETXX_011_LIBRARYFACADE_CLASS_H
#define DARKNETXX_011_LIBRARYFACADE_CLASS_H

#include <string>
#include <tuple>
#include <vector>

#include "ILib.h"
#include "ILogger.h"
#include "ITrainer.h"
#include "LibraryAPI.h"
#include "LibraryContext.h"

namespace darknetxx011
{

/**
 * @brief The entry point of the library.
 */
class DARKNETXX_LIB_API LibraryFacade
{
 public:
  /// @brief The names and the descriptions of the formats.
  using FormatsDescriptions = std::vector<std::tuple<std::string, std::string>>;

  virtual ~LibraryFacade() = default;
  LibraryFacade() = default;

  static LibraryContextPtr create_library_context();

  /**
   * @brief The work the context asks for: the network weights files
   * conversion, the detection (the detect_cfg one) or the training (the
   * train_cfg one).
   *
   * @return Returns the nullptr for the context asking for none.
   */
  static ILibPtr create_library(LibraryContextPtr ctx);

  /// @brief The trainer of the YOLO4 folders into the context.
  static ITrainerPtr create_trainer(LibraryContextPtr ctx);

  /// @brief The network weights files formats the conversion accepts.
  static FormatsDescriptions conversion_formats();

  /**
   * @brief The PNG image of the training chart the weights file keeps the
   * record of (the dxxwjz2 one): the chart the training draws of the losses
   * and the mAPs, redrawn.
   *
   * @return The empty image for the file of no record or unable to be read.
   */
  static std::vector<unsigned char> training_chart(const std::string& weights);

  /**
   * @brief Hands the logger of the library user over to the library, which
   * logs into it then. Call it before the other library calls.
   */
  static void init_logger(const logger::ILoggerPtr& appLogger);
};

}  // namespace darknetxx011

#endif  // DARKNETXX_011_LIBRARYFACADE_CLASS_H
