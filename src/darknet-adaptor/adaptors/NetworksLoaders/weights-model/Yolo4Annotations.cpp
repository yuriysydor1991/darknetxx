#include "src/darknet-adaptor/adaptors/NetworksLoaders/weights-model/Yolo4Annotations.h"

namespace darknet_adaptor::loaders
{

bool Yolo4Annotations::empty() const { return images.empty(); }

std::vector<std::string> Yolo4Annotations::training_paths() const
{
  return paths(false);
}

std::vector<std::string> Yolo4Annotations::validation_paths() const
{
  std::vector<std::string> validation = paths(true);

  return validation.empty() ? training_paths() : validation;
}

std::vector<std::string> Yolo4Annotations::paths(const bool validation) const
{
  std::vector<std::string> found;

  for (const Yolo4Image& image : images) {
    if (image.validation == validation) {
      found.push_back(image.path);
    }
  }

  return found;
}

}  // namespace darknet_adaptor::loaders
