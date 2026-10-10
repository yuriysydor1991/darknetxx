#include "src/lib/darknet-adaptor/adaptors/TrainingData/TrainingDataRegistry.h"

#include "src/log/log.h"

namespace darknet_adaptor::adaptors
{

void TrainingDataRegistry::publish(const loaders::Yolo4Annotations& annotations)
{
  std::lock_guard<std::mutex> lock{mutex()};

  lists() = {{TRAIN_LIST, annotations.training_paths()},
             {VALID_LIST, annotations.validation_paths()}};
  labels().clear();

  for (const loaders::Yolo4Image& image : annotations.images) {
    // the label file path the darknet reads the boxes of the image by
    const auto [label, added] = labels().emplace(
        loaders::Yolo4Image::label_of(image.path), image.boxes);

    // the image of both of the lists has the same boxes
    if (!added && label->second != image.boxes) {
      LOGW("The images of the same label file " << label->first
                                                << ", the first one's boxes");
    }
  }
}

void TrainingDataRegistry::withdraw()
{
  std::lock_guard<std::mutex> lock{mutex()};

  lists().clear();
  labels().clear();
}

bool TrainingDataRegistry::has(const std::string& name)
{
  std::lock_guard<std::mutex> lock{mutex()};

  return lists().count(name) > 0U;
}

bool TrainingDataRegistry::paths(const std::string& name,
                                 std::vector<std::string>& found)
{
  std::lock_guard<std::mutex> lock{mutex()};
  const auto list = lists().find(name);

  if (list == lists().end()) {
    return false;
  }

  found = list->second;

  return true;
}

bool TrainingDataRegistry::boxes(const std::string& labelpath,
                                 std::vector<loaders::Yolo4Box>& found)
{
  std::lock_guard<std::mutex> lock{mutex()};
  const auto label = labels().find(labelpath);

  if (label == labels().end()) {
    return false;
  }

  found = label->second;

  return true;
}

std::mutex& TrainingDataRegistry::mutex()
{
  static std::mutex registry;
  return registry;
}

TrainingDataRegistry::Lists& TrainingDataRegistry::lists()
{
  static Lists registry;
  return registry;
}

TrainingDataRegistry::Labels& TrainingDataRegistry::labels()
{
  static Labels registry;
  return registry;
}

}  // namespace darknet_adaptor::adaptors
