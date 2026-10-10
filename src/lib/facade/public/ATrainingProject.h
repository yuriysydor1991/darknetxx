#ifndef DARKNETXX_011_ATRAININGPROJECT_CLASS_H
#define DARKNETXX_011_ATRAININGPROJECT_CLASS_H

#include <filesystem>
#include <memory>
#include <string>

#include "LibraryAPI.h"
#include "LibraryContext.h"

namespace darknetxx011
{

/**
 * @brief The project a training trains: the network file of it, the weights
 * the training resumes from and the folder the training works in, the one of
 * the backup folder of the network and of the training charts.
 */
class DARKNETXX_LIB_API ATrainingProject
{
 public:
  virtual ~ATrainingProject() = default;

  /// @brief Tells the project has a network to train.
  bool valid() const;

  /// @brief The folder of the project, the working directory of it's training.
  const std::filesystem::path& path() const;

  /// @brief The network file the training trains: the cfg one, the project
  /// one keeping the network.
  const std::string& cfg() const;

  /// @brief The weights file a training resumes from, empty for none.
  const std::string& latest_weights() const;

  /// @brief The chart image of the latest training, empty for none yet.
  std::string chart() const;

  /**
   * @brief Makes the folder the working directory, creates the backup folder
   * of the network and makes the context to train the project files, the new
   * chart continuing the latest one.
   */
  bool enter(LibraryContext& ctx) const;

 protected:
  explicit ATrainingProject(std::filesystem::path nroot);

  /// @brief The absolute path, the empty one for the empty path.
  static std::filesystem::path absolute_of(const std::string& path);

  /// @brief Sets the network file to train and the weights it resumes from.
  void train(std::string ncfg, std::string nweights);

  /// @brief The class names file of the training, empty for the names of
  /// the network file.
  virtual std::string names() const = 0;

 private:
  std::filesystem::path root;
  std::string cfgFile;
  std::string weightsFile;
};

using ATrainingProjectPtr = std::shared_ptr<const ATrainingProject>;

}  // namespace darknetxx011

#endif  // DARKNETXX_011_ATRAININGPROJECT_CLASS_H
