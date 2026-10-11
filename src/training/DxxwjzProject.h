#ifndef DARKNETXX_PROJECT_DXXWJZPROJECT_CLASS_H
#define DARKNETXX_PROJECT_DXXWJZPROJECT_CLASS_H

#include <string>

#include "ATrainingProject.h"

namespace darknetxx_impl
{

/**
 * @brief The training project of the dxxwjz2p or the dxxwjz3p file (see the
 * Dxxwjz2pFormat and the Dxxwjz3pFormat): the network, the weights and the
 * annotations of the images in one file, which the training resumes from and
 * updates by it's latest weights. The folder of the file is the one of the
 * training.
 */
class DxxwjzProject : public darknetxx011::ATrainingProject
{
 public:
  explicit DxxwjzProject(const std::string& npath);

 protected:
  /// @brief None, the project keeps the class names.
  std::string names() const override;
};

}  // namespace darknetxx_impl

#endif  // DARKNETXX_PROJECT_DXXWJZPROJECT_CLASS_H
