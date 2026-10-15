#include "src/gui/net-editor/SectionSchemas.h"

#include <map>
#include <memory>

#include "src/gui/net-editor/AnchorsOption.h"
#include "src/gui/net-editor/BoundedOption.h"
#include "src/gui/net-editor/ChoiceOption.h"
#include "src/gui/net-editor/DivisorOption.h"
#include "src/gui/net-editor/HeadClassesOption.h"
#include "src/gui/net-editor/HeadFiltersOption.h"
#include "src/gui/net-editor/InputSizeOption.h"
#include "src/gui/net-editor/LayerIndexesOption.h"
#include "src/gui/net-editor/NumberOption.h"
#include "src/gui/net-editor/ScalesOption.h"
#include "src/gui/net-editor/StepsOption.h"
#include "src/gui/net-editor/TextOption.h"

namespace net_editor
{

namespace
{

using Options = std::vector<AOptionPtr>;

constexpr const double NONE = NumberOption::NONE;

AOptionPtr integer(const std::string& key, const std::string& def,
                   const std::string& about, double lowest = -NONE,
                   double highest = NONE)
{
  return std::make_shared<NumberOption>(key, def, about, true, false,
                                        NumberOption::Range{lowest, highest});
}

AOptionPtr real(const std::string& key, const std::string& def,
                const std::string& about, double lowest = -NONE,
                double highest = NONE)
{
  return std::make_shared<NumberOption>(key, def, about, false, false,
                                        NumberOption::Range{lowest, highest});
}

AOptionPtr flag(const std::string& key, const std::string& def,
                const std::string& about)
{
  return std::make_shared<ChoiceOption>(key, def, about,
                                        std::vector<std::string>{"0", "1"});
}

AOptionPtr choice(const std::string& key, const std::string& def,
                  const std::string& about, std::vector<std::string> choices)
{
  return std::make_shared<ChoiceOption>(key, def, about, std::move(choices));
}

AOptionPtr text(const std::string& key, const std::string& def,
                const std::string& about)
{
  return std::make_shared<TextOption>(key, def, about);
}

AOptionPtr activation(const std::string& def)
{
  return choice("activation", def, "The activation function of the outputs",
                {"leaky",
                 "mish",
                 "swish",
                 "linear",
                 "logistic",
                 "relu",
                 "relu6",
                 "elu",
                 "selu",
                 "gelu",
                 "tanh",
                 "hard_mish",
                 "relie",
                 "plse",
                 "hardtan",
                 "lhtan",
                 "ramp",
                 "revleaky",
                 "stair",
                 "loggy",
                 "normalize_channels",
                 "normalize_channels_softmax",
                 "normalize_channels_softmax_maxval"});
}

AOptionPtr batch_normalize()
{
  return flag("batch_normalize", "0",
              "1 normalizes the outputs of every mini batch (the layer keeps "
              "the scales, the rolling mean and variance arrays then)");
}

/// @brief The options every layer reads, described when the layer has them.
Options generic()
{
  return {
      integer("stopbackward", "0",
              "1 stops the backward pass at the layer, the layers before it "
              "aren't trained",
              0),
      flag("dontload", "0", "1 loads no weights of the layer"),
      flag("dontloadscales", "0",
           "1 loads no batch normalization arrays of the layer"),
      flag("train_only_bn", "0",
           "1 trains the batch normalization of the layers up to it alone"),
      real("learning_rate", "1", "The scale of the learning rate of the layer",
           0),
      flag("onlyforward", "0", "1 skips the backward pass of the layer"),
      flag("dont_update", "0", "1 keeps the weights of the layer as they are"),
      integer("burnin_update", "0",
              "The iterations of the burn in the layer isn't updated during",
              0),
  };
}

AOptionPtr layer_indexes(const std::string& key, bool list)
{
  return std::make_shared<LayerIndexesOption>(
      key, "The layers the layer takes the outputs of", list);
}

/// @brief The options of the [yolo] and the [Gaussian_yolo] layers.
Options yolo_options()
{
  return {
      std::make_shared<BoundedOption>(
          "mask", "",
          "The indexes of the anchors the layer detects the objects of", true,
          0, "num", 1, true),
      std::make_shared<AnchorsOption>(
          "The width and the height of every anchor box in the network input "
          "pixels"),
      std::make_shared<HeadClassesOption>(
          "20", "The classes the layer detects the objects of"),
      integer("num", "1", "The anchors of the anchors option", 1),
      real("jitter", "0.2",
           "The random crop and shift of the training images, the fraction "
           "of their size",
           0, 1),
      real("ignore_thresh", "0.5",
           "The IoU of the prediction and the truth over which the "
           "prediction isn't punished for no object",
           0, 1),
      real("truth_thresh", "1",
           "The IoU over which the prediction is trained as the truth one", 0,
           1),
      real("iou_thresh", "1",
           "The IoU of the truth and the anchors over which every one of them "
           "is trained, not the best one alone",
           0, 1),
      real("random", "0",
           "1 resizes the network every 10 iterations from the 1 / 1.4 to the "
           "1.4 of the size, the other value is the factor of the resize",
           0),
      real("scale_x_y", "1",
           "The scale of the box center predictions, 1.05 to 1.2 lets them "
           "reach the cell edges",
           1),
      choice("iou_loss", "mse", "The loss of the box coordinates",
             {"mse", "giou", "diou", "ciou"}),
      real("iou_normalizer", "0.75", "The weight of the box loss", 0),
      real("cls_normalizer", "1", "The weight of the class loss", 0),
      real("obj_normalizer", "1", "The weight of the objectness loss", 0),
      choice("nms_kind", "default",
             "The non maximum suppression of the detections",
             {"default", "greedynms", "diounms"}),
      real("beta_nms", "0.6", "The beta of the DIoU non maximum suppression",
           0),
      integer("max", "200", "The objects of an image the training takes", 1),
      flag("focal_loss", "0", "1 trains the classes by the focal loss"),
      flag("new_coords", "0",
           "1 predicts the boxes the scaled YOLOv4 way (the logistic "
           "activation before the layer)"),
      real("label_smooth_eps", "0",
           "The label smoothing of the class probabilities", 0, 1),
      real("resize", "1",
           "The resize of the training images of the jitter, the factor of "
           "it's range",
           0),
  };
}

std::map<std::string, SectionSchema> layer_schemas()
{
  std::map<std::string, SectionSchema> schemas;
  const auto add = [&schemas](std::initializer_list<const char*> names,
                              const SectionSchema& schema) {
    for (const char* name : names) {
      schemas.emplace(name, schema);
    }
  };

  add({"convolutional", "conv"},
      SectionSchema{
          "The convolution of the input by the filters",
          {std::make_shared<HeadFiltersOption>(
               "The filters of the layer, the channels of it's output"),
           integer("size", "1", "The width and the height of the filters", 1),
           integer("stride", "1",
                   "The step of the filters, the output is the input of the "
                   "size divided by it",
                   1),
           flag("pad", "0", "1 pads the input by the size / 2"),
           integer("padding", "0", "The padding of the input of no pad", 0),
           std::make_shared<DivisorOption>(
               "groups", "1",
               "The groups of the channels the filters convolve apart, the "
               "filters = groups of the depthwise convolution",
               "filters", 1),
           integer("dilation", "1", "The dilation of the filters", 1),
           activation("logistic"), batch_normalize(),
           integer("antialiasing", "0",
                   "1 blurs the output of the stride, 2 by the 2 x 2 kernel", 0,
                   2)},
          {{"batch_normalize", "1"},
           {"filters", "32"},
           {"size", "3"},
           {"stride", "1"},
           {"pad", "1"},
           {"activation", "leaky"}},
          generic()});

  add({"maxpool", "max"},
      SectionSchema{
          "The maxima of the windows of the input",
          {integer("size", "", "The window of the maxima, the stride of none",
                   1),
           integer("stride", "1",
                   "The step of the window, the output is the input of the "
                   "size divided by it",
                   1),
           integer("padding", "",
                   "The padding of the input, the size - 1 of none", 0),
           flag("maxpool_depth", "0",
                "1 takes the maxima across the channels instead"),
           integer("out_channels", "1",
                   "The output channels of the maxpool_depth", 1),
           integer("antialiasing", "0", "1 blurs the output of the stride", 0,
                   2)},
          {{"size", "2"}, {"stride", "2"}},
          generic()});

  add({"route"},
      SectionSchema{
          "The outputs of the layers joined along the channels",
          {layer_indexes("layers", true),
           integer("groups", "1",
                   "The groups of the channels of every input, the route "
                   "takes one of them",
                   1),
           std::make_shared<BoundedOption>(
               "group_id", "0", "The group of the channels the route takes",
               false, 0, "groups", 1, true)},
          {{"layers", "-1"}}});

  add({"shortcut"},
      SectionSchema{
          "The sum of the output of the previous layer and of the outputs of "
          "the layers (the residual connection)",
          {layer_indexes("from", true), activation("linear"),
           choice("weights_type", "none",
                  "The weights of the sum the layer learns",
                  {"none", "per_feature", "per_layer", "per_channel"}),
           choice("weights_normalization", "none",
                  "The normalization of the learned weights",
                  {"none", "relu", "avg_relu", "softmax"})},
          {{"from", "-3"}, {"activation", "linear"}},
          generic()});

  add({"upsample"},
      SectionSchema{
          "The input enlarged by the stride (the negative stride reduces "
          "it)",
          {integer("stride", "2", "The factor of the width and the height"),
           real("scale", "1", "The scale of the output values")},
          {{"stride", "2"}}});

  add({"yolo"},
      SectionSchema{
          "The detection of the objects of the anchors of the mask (the "
          "YOLOv3 and the YOLOv4 layer)",
          yolo_options(),
          {{"mask", "0,1,2"},
           {"anchors", "10,14,23,27,37,58,81,82,135,169,344,319"},
           {"classes", "1"},
           {"num", "6"},
           {"jitter", "0.3"},
           {"ignore_thresh", "0.7"},
           {"truth_thresh", "1"},
           {"random", "1"}}});

  Options gaussian = yolo_options();

  gaussian.push_back(
      real("uc_normalizer", "1", "The weight of the uncertainty loss", 0));
  gaussian.push_back(choice("yolo_point", "center",
                            "The point of the box the layer predicts",
                            {"center", "left_top", "right_bottom"}));

  add({"Gaussian_yolo"},
      SectionSchema{"The [yolo] layer of the uncertainty of the boxes",
                    gaussian,
                    {{"mask", "0,1,2"},
                     {"anchors", "10,14,23,27,37,58,81,82,135,169,344,319"},
                     {"classes", "1"},
                     {"num", "6"},
                     {"jitter", "0.3"},
                     {"ignore_thresh", "0.7"},
                     {"truth_thresh", "1"},
                     {"random", "1"}}});

  add({"region"},
      SectionSchema{
          "The detection of the objects of the anchors (the YOLOv2 layer)",
          {std::make_shared<AnchorsOption>(
               "The width and the height of every anchor box in the grid "
               "cells"),
           std::make_shared<HeadClassesOption>(
               "20", "The classes the layer detects the objects of"),
           integer("coords", "4", "The coordinates of a box", 1),
           integer("num", "1", "The anchors of the anchors option", 1),
           flag("softmax", "0", "1 takes the softmax of the classes"),
           real("jitter", "0.2", "The random crop and shift of the images", 0,
                1),
           flag("rescore", "0", "1 trains the objectness of the IoU"),
           real("object_scale", "1", "The weight of the objectness loss", 0),
           real("noobject_scale", "1", "The weight of the no object loss", 0),
           real("class_scale", "1", "The weight of the class loss", 0),
           real("coord_scale", "1", "The weight of the box loss", 0),
           real("thresh", "0.5",
                "The IoU over which the prediction isn't punished for no "
                "object",
                0, 1),
           real("random", "0", "1 resizes the network during the training", 0),
           flag("bias_match", "0",
                "1 trains the box sizes of the anchors at first"),
           flag("absolute", "0", "1 keeps the boxes of the absolute anchors")},
          {{"anchors", "1.08,1.19,3.42,4.41,6.63,11.38,9.42,5.11,16.62,10.52"},
           {"classes", "1"},
           {"coords", "4"},
           {"num", "5"},
           {"softmax", "1"},
           {"jitter", "0.2"},
           {"rescore", "1"}}});

  add({"detection"},
      SectionSchema{
          "The detection of the objects in the cells of a grid (the YOLOv1 "
          "layer)",
          {integer("classes", "1", "The classes the layer detects", 1),
           integer("coords", "1", "The coordinates of a box", 1),
           integer("num", "1", "The boxes of a cell", 1),
           integer("side", "7", "The cells of the side of the grid", 1),
           flag("softmax", "0", "1 takes the softmax of the classes"),
           flag("sqrt", "0", "1 predicts the square roots of the box sizes"),
           flag("rescore", "0", "1 trains the objectness of the IoU"),
           real("jitter", "0.2", "The random crop and shift of the images", 0,
                1),
           real("object_scale", "1", "The weight of the objectness loss", 0),
           real("noobject_scale", "1", "The weight of the no object loss", 0),
           real("class_scale", "1", "The weight of the class loss", 0),
           real("coord_scale", "1", "The weight of the box loss", 0)},
          {{"classes", "1"}, {"coords", "4"}, {"num", "2"}, {"side", "7"}}});

  add({"connected", "conn"},
      SectionSchema{
          "The dense layer: every output of the weighted sum of every input",
          {integer("output", "1", "The outputs of the layer", 1),
           activation("logistic"), batch_normalize()},
          {{"output", "10"}, {"activation", "linear"}},
          generic()});

  add({"softmax", "soft"},
      SectionSchema{
          "The probabilities of the classes of the inputs (the classifier "
          "end)",
          {integer("groups", "1", "The groups of the inputs of their own sum",
                   1),
           real("temperature", "1", "The temperature of the softmax", 0),
           text("tree", "", "The hierarchy file of the classes"),
           flag("noloss", "0", "1 trains the layer of no loss")},
          {}});

  add({"dropout"},
      SectionSchema{
          "The random drop of the inputs during the training",
          {real("probability", "0.2",
                "The probability of the input to be dropped", 0, 1),
           flag("dropblock", "0", "1 drops the blocks of the input"),
           real("dropblock_size_rel", "0",
                "The block size of the dropblock, the fraction of "
                "the input",
                0, 1),
           integer("dropblock_size_abs", "0",
                   "The block size of the dropblock in the pixels", 0)},
          {{"probability", "0.5"}}});

  add({"avgpool", "avg"},
      SectionSchema{"The average of every channel of the input", {}});

  add({"local_avgpool", "local_avg"},
      SectionSchema{
          "The averages of the windows of the input",
          {integer("size", "", "The window of the averages, the stride of none",
                   1),
           integer("stride", "1", "The step of the window", 1),
           integer("padding", "",
                   "The padding of the input, the size - 1 of none", 0)},
          {{"size", "2"}, {"stride", "2"}}});

  add({"batchnorm"}, SectionSchema{"The batch normalization of the input", {}});

  add({"activation"}, SectionSchema{"The activation function of the input",
                                    {activation("linear")},
                                    {{"activation", "leaky"}}});

  add({"scale_channels"},
      SectionSchema{
          "The channels of the previous layer scaled by the outputs of the "
          "layer (the squeeze and excitation)",
          {layer_indexes("from", false),
           flag("scale_wh", "0", "1 scales the width and the height instead"),
           activation("linear")},
          {{"from", "-3"}}});

  add({"sam"},
      SectionSchema{
          "The output of the previous layer multiplied by the outputs of the "
          "layer (the spatial attention)",
          {layer_indexes("from", false), activation("linear")},
          {{"from", "-2"}}});

  add({"reorg3d", "reorg"},
      SectionSchema{
          "The pixels of the windows moved into the channels",
          {integer("stride", "1", "The window of the moved pixels", 1),
           flag("reverse", "0", "1 moves them back")},
          {{"stride", "2"}}});

  add({"crop"},
      SectionSchema{
          "The random crop of the input images",
          {integer("crop_height", "1", "The height of the crop", 1),
           integer("crop_width", "1", "The width of the crop", 1),
           flag("flip", "0", "1 flips the images at random"),
           real("angle", "0", "The random rotation in the degrees"),
           real("saturation", "1", "The random saturation", 0),
           real("exposure", "1", "The random exposure", 0),
           flag("noadjust", "0", "1 keeps the image values as they are")},
          {}});

  add({"cost"},
      SectionSchema{
          "The loss of the outputs and the truth",
          {choice("type", "sse", "The loss", {"sse", "masked", "smooth"}),
           real("scale", "1", "The weight of the loss", 0),
           real("ratio", "0", "The ratio of the hardest losses", 0, 1)},
          {}});

  add({"local"},
      SectionSchema{"The convolution of the filters of every position",
                    {integer("filters", "1", "The filters of the layer", 1),
                     integer("size", "1",
                             "The width and the height of the "
                             "filters",
                             1),
                     integer("stride", "1", "The step of the filters", 1),
                     flag("pad", "0", "1 pads the input by the size / 2"),
                     activation("logistic")},
                    {{"filters", "32"}, {"size", "3"}, {"pad", "1"}},
                    generic()});

  add({"implicit"},
      SectionSchema{
          "The learned implicit knowledge values (the YOLOR)",
          {integer("filters", "128", "The values of the layer", 1),
           integer("atoms", "1", "The atoms of a value", 1),
           real("mean", "0", "The mean of the initial values"),
           real("std", "0.2", "The deviation of the initial values", 0)},
          {{"filters", "128"}}});

  add({"empty", "silence"}, SectionSchema{"The input as it is", {}});

  add({"rnn"},
      SectionSchema{"The recurrent layer",
                    {integer("output", "1", "The outputs of the layer", 1),
                     integer("hidden", "1", "The hidden state values", 1),
                     activation("logistic"), batch_normalize()},
                    {{"output", "64"}, {"hidden", "64"}},
                    generic()});

  add({"gru"},
      SectionSchema{"The gated recurrent unit layer",
                    {integer("output", "1", "The outputs of the layer", 1),
                     batch_normalize()},
                    {{"output", "64"}},
                    generic()});

  add({"lstm"},
      SectionSchema{"The long short term memory layer",
                    {integer("output", "1", "The outputs of the layer", 1),
                     batch_normalize()},
                    {{"output", "64"}},
                    generic()});

  add({"crnn"},
      SectionSchema{
          "The convolutional recurrent layer",
          {integer("output", "1", "The output channels", 1),
           integer("hidden", "1", "The hidden state channels", 1),
           integer("size", "3", "The width and the height of the filters", 1),
           integer("stride", "1", "The step of the filters", 1),
           flag("pad", "0", "1 pads the input by the size / 2"),
           activation("logistic"), batch_normalize()},
          {{"output", "32"}, {"hidden", "32"}, {"pad", "1"}},
          generic()});

  add({"conv_lstm"},
      SectionSchema{
          "The convolutional long short term memory layer",
          {integer("output", "1", "The output channels", 1),
           integer("size", "3", "The width and the height of the filters", 1),
           integer("stride", "1", "The step of the filters", 1),
           flag("pad", "0", "1 pads the input by the size / 2"),
           activation("linear"), batch_normalize(),
           flag("peephole", "0", "1 adds the peephole connections"),
           choice("lstm_activation", "tanh", "The activation of the cell state",
                  {"tanh", "linear", "logistic", "relu", "leaky", "mish"})},
          {{"output", "32"}, {"pad", "1"}},
          generic()});

  add({"normalization", "lrn"},
      SectionSchema{"The local response normalization",
                    {real("alpha", "0.0001", "The alpha of the normalization"),
                     real("beta", "0.75", "The beta of the normalization"),
                     real("kappa", "1", "The kappa of the normalization"),
                     integer("size", "5", "The channels of the window", 1)},
                    {}});

  add({"contrastive"},
      SectionSchema{
          "The contrastive loss of the features",
          {integer("classes", "1000", "The classes of the features", 1),
           real("temperature", "1", "The temperature of the loss", 0),
           integer("yolo_layer", "0", "The [yolo] layer of the features")},
          {}});

  add({"history"},
      SectionSchema{"The outputs of the previous iterations",
                    {integer("history_size", "4", "The iterations kept", 1)},
                    {}});

  return schemas;
}

}  // namespace

const SectionSchema& SectionSchemas::net()
{
  static const SectionSchema schema{
      "The network and it's training",
      {integer("batch", "1",
               "The images of a training iteration (the weights are updated "
               "once of them)",
               1),
       std::make_shared<DivisorOption>(
           "subdivisions", "1",
           "The mini batches of the batch: the batch / subdivisions images go "
           "through the network at once, more of them take less memory",
           "batch", 1),
       std::make_shared<InputSizeOption>(
           "width", "The width of the network input the images are resized to"),
       std::make_shared<InputSizeOption>(
           "height",
           "The height of the network input the images are resized to"),
       integer("channels", "",
               "The channels of the input images: 3 of the color ones, 1 of "
               "the gray ones",
               1),
       integer("inputs", "",
               "The input values of the network of no image input (the width "
               "* height * channels otherwise)",
               1),
       integer("max_batches", "0",
               "The iterations of the training: about 2000 * classes and the "
               "count of the training images at least",
               0),
       real("learning_rate", "0.001", "The learning rate of the training", 0),
       std::make_shared<BoundedOption>(
           "burn_in", "0",
           "The first iterations of the learning rate rising up to the "
           "learning_rate, which keeps the training from diverging at first",
           false, 0, "max_batches", 0, false),
       choice("policy", "constant",
              "The learning rate policy: the steps one scales it at the steps",
              {"constant", "steps", "step", "exp", "poly", "sigmoid", "random",
               "sgdr"}),
       std::make_shared<StepsOption>(
           "The iterations the learning rate is scaled at by the steps "
           "policy, usually the 80% and the 90% of the max_batches"),
       std::make_shared<ScalesOption>(
           "The factors of the learning rate at the steps"),
       real("momentum", "0.9", "The momentum of the weights updates", 0, 1),
       real("decay", "0.0001",
            "The weight decay, which keeps the weights from growing", 0),
       real("angle", "0",
            "The random rotation of the training images up to the degrees "
            "(the classifiers alone)",
            0, 180),
       real("saturation", "1",
            "The random saturation of the training images, from the 1 / "
            "saturation to the saturation",
            1),
       real("exposure", "1",
            "The random exposure of the training images, from the 1 / "
            "exposure to the exposure",
            1),
       real("hue", "0",
            "The random hue shift of the training images, the fraction of the "
            "hue circle",
            0, 1),
       flag("flip", "1", "1 flips the training images at random"),
       flag("mosaic", "0",
            "1 trains the mosaics of four images (the YOLOv4 augmentation)"),
       flag("cutmix", "0", "1 trains the images of the cut and mixed parts"),
       integer("mixup", "0", "1 trains the images mixed up of two", 0, 4),
       integer("blur", "0",
               "1 blurs the background of the training images at random, the "
               "other value is the blur size",
               0),
       flag("letter_box", "0",
            "1 resizes the training images keeping their aspect ratio"),
       std::make_shared<BoundedOption>(
           "sequential_subdivisions", "",
           "The subdivisions of the sequence of the recurrent networks, the "
           "subdivisions of none",
           false, 1, "subdivisions", 1, false),
       integer("time_steps", "1",
               "The steps of the sequence of the recurrent networks", 1),
       real("max_chart_loss", "20", "The highest loss of the training chart",
            0),
       integer("saveweights", "",
               "The iterations of a numbered weights file save: 1000 of the "
               "max_batches below 10000, 10000 otherwise",
               1),
       integer("savelast", "100",
               "The iterations of a save of the _last weights file", 1),
       integer("savechart", "0",
               "The iterations of a save of the training chart, 0 leaves it "
               "to the --save-charts-every-iter",
               0),
       text("backup", "backup",
            "The folder of the weights the training saves, relative to the "
            "project"),
       text("train", "data/train.txt",
            "The training images list of the YOLO4 folder (the project trains "
            "the images of it's annotations)"),
       text("valid", "",
            "The mAP images list of the YOLO4 folder, the train one of none"),
       text("names", "data/names.list",
            "The class names file of the YOLO4 folder (the project keeps the "
            "class names)")},
      {},
      {integer("step", "1",
               "The iterations of a learning rate scale of the step and the "
               "sigmoid policies",
               1),
       real("scale", "1", "The learning rate scale of the step policy", 0),
       real("gamma", "1",
            "The learning rate factor of the exp and the sigmoid policies", 0),
       real("power", "4", "The learning rate power of the poly policy", 0),
       real("learning_rate_min", "0.00001",
            "The lowest learning rate of the sgdr policy", 0),
       integer("sgdr_cycle", "",
               "The iterations of the first cycle of the sgdr policy, the "
               "max_batches of none",
               1),
       integer("sgdr_mult", "2", "The factor of the cycles of the sgdr policy",
               1),
       flag("adam", "0", "1 updates the weights by the Adam optimizer"),
       real("B1", "0.9", "The beta 1 of the Adam optimizer", 0, 1),
       real("B2", "0.999", "The beta 2 of the Adam optimizer", 0, 1),
       real("eps", "0.000001", "The epsilon of the Adam optimizer", 0),
       real("label_smooth_eps", "0",
            "The label smoothing of the class probabilities", 0, 1),
       real("ema_alpha", "0",
            "The alpha of the exponential moving average of the weights", 0, 1),
       real("aspect", "1", "The random aspect ratio of the training images", 0),
       integer("min_crop", "", "The smallest random crop of the classifiers",
               1),
       integer("max_crop", "", "The largest random crop of the classifiers", 1),
       integer("gaussian_noise", "0", "1 adds the random noise to the images",
               0),
       real("resize_step", "32", "The step of the random resize of the network",
            1)}};

  return schema;
}

const SectionSchema& SectionSchemas::of(const std::string& type)
{
  static const std::map<std::string, SectionSchema> schemas = layer_schemas();
  static const SectionSchema unknown{
      "The layer the editor knows nothing of, it's options are kept as they "
      "are",
      {}};
  const auto found = schemas.find(type);

  return found == schemas.end() ? unknown : found->second;
}

const std::vector<std::string>& SectionSchemas::layer_types()
{
  static const std::vector<std::string> types{
      "convolutional", "maxpool",   "route",         "shortcut",
      "upsample",      "yolo",      "connected",     "softmax",
      "dropout",       "avgpool",   "batchnorm",     "activation",
      "Gaussian_yolo", "region",    "local_avgpool", "scale_channels",
      "sam",           "reorg3d",   "reorg",         "crop",
      "cost",          "local",     "implicit",      "empty",
      "rnn",           "gru",       "lstm",          "crnn",
      "conv_lstm",     "detection", "normalization", "contrastive",
      "history"};

  return types;
}

}  // namespace net_editor
