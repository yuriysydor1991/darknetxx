#include "src/darknet-adaptor/adaptors/NetworksLoaders/weights-model/Yolo4Image.h"

#include "utils.h"

namespace darknet_adaptor::loaders
{

std::string Yolo4Image::label_of(const std::string& path)
{
  // the label path is never longer than the image one
  char label[MAX_PATH + 1U] = {};

  if (path.size() > MAX_PATH) {
    return {};
  }

  replace_image_to_label(path.c_str(), label);

  return label;
}

}  // namespace darknet_adaptor::loaders
