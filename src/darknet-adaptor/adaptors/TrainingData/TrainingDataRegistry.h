#ifndef DARKNET_CXX_PORT_PROJECT_TRAINING_DATA_REGISTRY_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_TRAINING_DATA_REGISTRY_CLASS_H

#include <map>
#include <mutex>
#include <string>
#include <vector>

#include "src/darknet-adaptor/adaptors/NetworksLoaders/weights-model/Yolo4Annotations.h"

namespace darknet_adaptor::adaptors
{

/**
 * @brief The image lists and the labels of the training of a dxxwjz2p
 * project, which the darknet C code reads instead of the list and the label
 * files of the YOLO4 folder: the lists of the TRAIN_LIST and the VALID_LIST
 * names and the boxes of the label file paths of the images (see the
 * TrainingDataCApi). One training of a project publishes them at a time,
 * the registry is empty otherwise and the files are read.
 */
class TrainingDataRegistry
{
 public:
  /// @brief The names of the lists of the training and the mAP images.
  inline static const std::string TRAIN_LIST{"dxxwjz2p:train"};
  inline static const std::string VALID_LIST{"dxxwjz2p:valid"};

  /// @brief Keeps the lists and the labels of the annotations.
  static void publish(const loaders::Yolo4Annotations& annotations);

  /// @brief Forgets the published ones.
  static void withdraw();

  /// @brief Tells there is the list of the name.
  static bool has(const std::string& name);

  /// @brief The paths of the list of the name, false for none.
  static bool paths(const std::string& name, std::vector<std::string>& found);

  /// @brief The boxes of the label file path, false for none.
  static bool boxes(const std::string& labelpath,
                    std::vector<loaders::Yolo4Box>& found);

 private:
  using Lists = std::map<std::string, std::vector<std::string>>;
  using Labels = std::map<std::string, std::vector<loaders::Yolo4Box>>;

  static std::mutex& mutex();
  static Lists& lists();
  static Labels& labels();
};

}  // namespace darknet_adaptor::adaptors

#endif  // DARKNET_CXX_PORT_PROJECT_TRAINING_DATA_REGISTRY_CLASS_H
