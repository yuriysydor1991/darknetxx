#ifndef DARKNETXX_011_LIBRARYFACADE_CLASS_H
#define DARKNETXX_011_LIBRARYFACADE_CLASS_H

#include <string>
#include <tuple>
#include <vector>

#include "ATrainingProject.h"
#include "ILib.h"
#include "ILogger.h"
#include "IRecognizer.h"
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

  /// @brief The trainer of the training projects into the context.
  static ITrainerPtr create_trainer(LibraryContextPtr ctx);

  /// @brief The training project of the path: the one of the dxxwjz2p or
  /// the dxxwjz3p project file, the YOLO4 folder one of the rest.
  static ATrainingProjectPtr open_training_project(const std::string& path);

  /// @brief The recognizer of the images by the networks of the weights files.
  static IRecognizerPtr create_recognizer();

  /// @brief The network weights files formats the conversion accepts and
  /// the YOLO4 folder of the training project.
  static FormatsDescriptions conversion_formats();

  /// @brief The file name extensions of the network weights files of every
  /// format the training and the recognition read, e.g. ".dxxwjz2".
  static std::vector<std::string> weights_extensions();

  /**
   * @brief The PNG image of the training chart the weights file keeps the
   * record of (the dxxwjz2 one): the chart the training draws of the losses
   * and the mAPs, redrawn.
   *
   * @return The empty image for the file of no record or unable to be read.
   */
  static std::vector<unsigned char> training_chart(const std::string& weights);

  /**
   * @brief The darknet (YOLO4) cfg text of the network the weights file keeps
   * (the dxxwjz2, the dxxwjz2p, the dxxwjz3 and the dxxwjz3p ones): the cfg
   * text the dxxwjz2 file keeps as it is, the one of the JSON options of the
   * dxxwjz3 file of no comments.
   *
   * @return The empty text for the file of no network cfg or unable to be
   * read.
   */
  static std::string network_cfg(const std::string& weights);

  /**
   * @brief The network description the file keeps (the dxxwjz2, the
   * dxxwjz2p, the dxxwjz3 and the dxxwjz3p ones): the network_cfg() text and
   * the class names, the name of a class at the class index.
   *
   * @return False for the file of no network description or unable to be
   * read.
   */
  static bool network_description(const std::string& weights, std::string& cfg,
                                  std::vector<std::string>& classes);

  /**
   * @brief Saves the edited network into the file of the format of it's file
   * name extension (the dxxwjz3p project, for example) through the temporary
   * file: the annotations, the training record and the weights of the
   * source go on, but the weights of the layers the edit changed, which are
   * dropped (the network to train of the weights kept).
   *
   * @param source The file the rest comes of, empty for none (the network
   * to train of no annotations).
   * @param destination The file to write, the source one included.
   * @param cfg The darknet cfg text of the network, which the darknet builds
   * the network of.
   * @param classes The class names, the name of a class at the class index.
   * @param message Out: the reason of the failure, or the weights the save
   * dropped, empty for none.
   *
   * @return False on any failure, which leaves the destination untouched.
   */
  static bool save_network(const std::string& source,
                           const std::string& destination,
                           const std::string& cfg,
                           const std::vector<std::string>& classes,
                           std::string& message);

  /**
   * @brief Hands the logger of the library user over to the library, which
   * logs into it then. Call it before the other library calls.
   */
  static void init_logger(const logger::ILoggerPtr& appLogger);
};

}  // namespace darknetxx011

#endif  // DARKNETXX_011_LIBRARYFACADE_CLASS_H
