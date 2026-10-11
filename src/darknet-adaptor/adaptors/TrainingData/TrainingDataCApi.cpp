/**
 * @brief The darknet C API of the image lists and the labels of the training
 * of a dxxwjz2p project, which the get_paths() and the read_boxes() of the
 * darknet C code ask before they read the files (see the
 * TrainingDataRegistry).
 */

#include <algorithm>
#include <cstring>
#include <string>
#include <vector>

#include "data.h"
#include "src/darknet-adaptor/adaptors/TrainingData/TrainingDataRegistry.h"
#include "src/darknet-adaptor/adaptors/orig-includes.h"

using darknet_adaptor::adaptors::TrainingDataRegistry;
using darknet_adaptor::loaders::Yolo4Box;

extern "C" {

list* get_training_data_paths(const char* name)
{
  std::vector<std::string> paths;

  if (name == nullptr || !TrainingDataRegistry::paths(name, paths)) {
    return nullptr;
  }

  list* lines = make_list();

  for (const std::string& path : paths) {
    auto* line = static_cast<char*>(xcalloc(path.size() + 1U, sizeof(char)));

    std::memcpy(line, path.c_str(), path.size() + 1U);
    list_insert(lines, line);
  }

  return lines;
}

int has_training_data_paths(const char* name)
{
  return name != nullptr && TrainingDataRegistry::has(name) ? 1 : 0;
}

box_label* read_training_data_boxes(const char* labelpath, int* n)
{
  // the track ids of the read_boxes(), of the hash of the label file path
  static constexpr const unsigned long MAX_OBJ_IMG = 4000UL;
  std::vector<Yolo4Box> boxes;

  if (labelpath == nullptr || !TrainingDataRegistry::boxes(labelpath, boxes)) {
    return nullptr;
  }

  const int hash = static_cast<int>(
      (custom_hash(const_cast<char*>(labelpath)) % MAX_OBJ_IMG) * MAX_OBJ_IMG);
  // an array of one at least, the read_boxes() way
  auto* found = static_cast<box_label*>(
      xcalloc(std::max<std::size_t>(1U, boxes.size()), sizeof(box_label)));

  for (std::size_t i = 0U; i < boxes.size(); ++i) {
    const Yolo4Box& b = boxes[i];

    found[i] = box_label{b.id,          hash + static_cast<int>(i),
                         b.x,           b.y,
                         b.w,           b.h,
                         b.x - b.w / 2, b.x + b.w / 2,
                         b.y - b.h / 2, b.y + b.h / 2};
  }

  *n = static_cast<int>(boxes.size());

  return found;
}

}  // extern "C"
