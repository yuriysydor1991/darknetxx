#ifndef DARKNETXX_011_YOLO4FOLDER_CLASS_H
#define DARKNETXX_011_YOLO4FOLDER_CLASS_H

#include <filesystem>
#include <string>

#include "LibraryAPI.h"
#include "LibraryContext.h"

namespace darknetxx011
{

/**
 * @brief The YOLO4 training folder, the one the ImagesAnnotator exports: the
 * network cfg/ *.cfg file, the data/obj.names class names, the training lists
 * the cfg names relative to the folder and the backup/ weights.
 */
class DARKNETXX_LIB_API Yolo4Folder
{
 public:
  explicit Yolo4Folder(const std::string& npath);

  /// @brief Tells the folder has a network cfg file to train.
  bool valid() const;

  const std::filesystem::path& path() const;
  const std::string& cfg() const;

  /// @brief The weights file a training resumes from, empty for none.
  const std::string& latest_weights() const;

  /// @brief The chart image of the latest training, empty for none yet.
  std::string chart() const;

  /**
   * @brief Makes the folder the working directory, which the training lists
   * of the cfg are relative to, creates it's backup folder and makes the
   * context to train the folder files, the new chart continuing the latest
   * one.
   */
  bool enter(LibraryContext& ctx) const;

 private:
  /// @brief The darknet base name of the cfg file, the one of it's weights
  /// and chart files.
  std::string base() const;

  static std::string find_cfg(const std::filesystem::path& cfgDir);

  std::filesystem::path backup_dir() const;

  /// @brief The newest last weights of the backup or the newest other ones of
  /// the cfg base name, of every weights file format.
  std::string find_latest_weights() const;

  static bool is_weights(const std::filesystem::path& path);

  std::filesystem::path root;
  std::string cfgFile;
  std::string weightsFile;
};

}  // namespace darknetxx011

#endif  // DARKNETXX_011_YOLO4FOLDER_CLASS_H
