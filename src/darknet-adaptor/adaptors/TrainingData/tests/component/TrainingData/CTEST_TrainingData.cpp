#include <gtest/gtest.h>

#include <algorithm>
#include <cstdlib>
#include <string>
#include <vector>

#include "data.h"
#include "src/darknet-adaptor/adaptors/TrainingData/TrainingDataRegistry.h"
#include "src/darknet-adaptor/adaptors/orig-includes.h"

using darknet_adaptor::adaptors::TrainingDataRegistry;
using darknet_adaptor::loaders::Yolo4Annotations;
using darknet_adaptor::loaders::Yolo4Box;
using darknet_adaptor::loaders::Yolo4Image;

namespace
{

Yolo4Image image_of(const std::string& path, const std::vector<Yolo4Box>& boxes,
                    const bool validation)
{
  Yolo4Image result;
  result.path = path;
  result.boxes = boxes;
  result.validation = validation;
  return result;
}

/// @brief The paths of the darknet list, which is freed.
std::vector<std::string> paths_of(list* lines)
{
  std::vector<std::string> paths;

  if (lines == nullptr) {
    return paths;
  }

  for (node* line = lines->front; line != nullptr; line = line->next) {
    paths.emplace_back(static_cast<char*>(line->val));
  }

  free_list_contents(lines);
  free_list(lines);

  return paths;
}

}  // namespace

class CTEST_TrainingData : public testing::Test
{
 public:
  ~CTEST_TrainingData() override { TrainingDataRegistry::withdraw(); }

  const std::vector<Yolo4Box> car{Yolo4Box{1, 0.5f, 0.25f, 0.25f, 0.5f},
                                  Yolo4Box{0, 0.75f, 0.75f, 0.5f, 0.5f}};
};

TEST_F(CTEST_TrainingData, darknet_reads_the_published_lists_and_labels)
{
  Yolo4Annotations annotations;
  annotations.images = {image_of("/data/a.jpg", car, false),
                        image_of("/data/b.png", {}, true),
                        image_of("/data/a.jpg", car, true)};

  EXPECT_EQ(get_training_data_paths(TrainingDataRegistry::TRAIN_LIST.c_str()),
            nullptr);

  TrainingDataRegistry::publish(annotations);

  EXPECT_EQ(paths_of(get_paths(
                const_cast<char*>(TrainingDataRegistry::TRAIN_LIST.c_str()))),
            std::vector<std::string>{"/data/a.jpg"});
  EXPECT_EQ(paths_of(get_paths(
                const_cast<char*>(TrainingDataRegistry::VALID_LIST.c_str()))),
            (std::vector<std::string>{"/data/b.png", "/data/a.jpg"}));
  EXPECT_EQ(has_training_data_paths(TrainingDataRegistry::VALID_LIST.c_str()),
            1);
  EXPECT_EQ(has_training_data_paths("data/train.txt"), 0);

  // the boxes of the label file paths of the images, the read_boxes() way
  int count = -1;
  box_label* boxes = read_boxes(const_cast<char*>("/data/a.txt"), &count);

  ASSERT_EQ(count, 2);
  EXPECT_EQ(boxes[0].id, 1);
  EXPECT_EQ(boxes[1].id, 0);
  EXPECT_EQ(boxes[1].track_id, boxes[0].track_id + 1);
  EXPECT_EQ(boxes[0].x, 0.5f);
  EXPECT_EQ(boxes[0].h, 0.5f);
  EXPECT_EQ(boxes[0].left, 0.375f);
  EXPECT_EQ(boxes[0].right, 0.625f);
  EXPECT_EQ(boxes[0].top, 0.f);
  EXPECT_EQ(boxes[0].bottom, 0.5f);
  free(boxes);

  boxes = read_training_data_boxes("/data/b.txt", &count);

  ASSERT_NE(boxes, nullptr);
  EXPECT_EQ(count, 0);
  free(boxes);

  EXPECT_EQ(read_training_data_boxes("/data/c.txt", &count), nullptr);

  // the files of the lists and the labels once the training is over
  TrainingDataRegistry::withdraw();

  EXPECT_EQ(get_training_data_paths(TrainingDataRegistry::TRAIN_LIST.c_str()),
            nullptr);
  EXPECT_EQ(read_training_data_boxes("/data/a.txt", &count), nullptr);
}

TEST_F(CTEST_TrainingData, classifier_trains_the_shares_of_the_boxes_classes)
{
  // the x, the y, the w, the h, the class and the track id of the boxes of the
  // fill_truth_detection() of the 3 images: of the 3 boxes, of 1 and of none
  const std::vector<std::vector<float>> truths{
      {.5f, .5f, 1, 1, 2, 0,       // the class 2
       .2f, .2f, .1f, .1f, 0, 1,   // the 0
       .7f, .7f, .1f, .1f, 2, 2},  // the 2 again
      {.5f, .5f, 1, 1, 1, 0},
      {}};
  matrix boxes = make_matrix(3, 3 * 6);

  for (int row = 0; row < boxes.rows; ++row) {
    std::copy(truths[static_cast<size_t>(row)].begin(),
              truths[static_cast<size_t>(row)].end(), boxes.vals[row]);
  }

  matrix classes = classes_of_boxes(boxes, 6, 3);
  const std::vector<std::vector<float>> shares{
      {1.f / 3, 0, 2.f / 3}, {0, 1, 0}, {0, 0, 0}};

  ASSERT_EQ(classes.rows, 3);
  ASSERT_EQ(classes.cols, 3);

  for (int row = 0; row < classes.rows; ++row) {
    EXPECT_EQ(std::vector<float>(classes.vals[row], classes.vals[row] + 3),
              shares[static_cast<size_t>(row)])
        << row;
  }

  free_matrix(boxes);
  free_matrix(classes);
}

TEST_F(CTEST_TrainingData, validation_list_is_the_training_one_for_none)
{
  Yolo4Annotations annotations;
  annotations.images = {image_of("/data/a.jpg", car, false)};

  TrainingDataRegistry::publish(annotations);

  EXPECT_EQ(paths_of(get_training_data_paths(
                TrainingDataRegistry::VALID_LIST.c_str())),
            std::vector<std::string>{"/data/a.jpg"});
}
