#ifndef DARKNETXX_012_YOLO4FOLDER_CLASS_H
#define DARKNETXX_012_YOLO4FOLDER_CLASS_H

#include <string>

#include "ATrainingProject.h"
#include "LibraryAPI.h"

namespace darknetxx012
{

/**
 * @brief The YOLO4 training folder, the one the ImagesAnnotator exports: the
 * network cfg/ *.cfg file, the data/obj.names class names, the training lists
 * the cfg names relative to the folder and the backup/ weights, the newest
 * of which the training resumes from.
 */
class DARKNETXX_LIB_API Yolo4Folder : public ATrainingProject
{
 public:
  explicit Yolo4Folder(const std::string& npath);

 protected:
  /// @brief The data/obj.names one, empty for none.
  std::string names() const override;
};

}  // namespace darknetxx012

#endif  // DARKNETXX_012_YOLO4FOLDER_CLASS_H
