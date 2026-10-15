#include <gtest/gtest.h>

#include <string>
#include <vector>

#include "src/gui/net-editor/NetworkModel.h"
#include "src/gui/net-editor/NetworkValidator.h"
#include "src/gui/net-editor/OptionPresenter.h"
#include "src/gui/net-editor/SectionSchemas.h"

using namespace net_editor;

namespace
{

/// @brief The yolov3-tiny head of the two classes, of the route of the
/// upsampled layer and of the layer 4.
const std::string YOLO =
    "[net]\nbatch=64\nsubdivisions=16\nwidth=416\nheight=416\nchannels=3\n"
    "max_batches=6000\npolicy=steps\nsteps=4800,5400\nscales=0.1,0.1\n\n"
    "[convolutional]\nfilters=16\nsize=3\nstride=1\npad=1\nactivation=leaky\n\n"
    "[maxpool]\nsize=2\nstride=2\n\n"
    "[maxpool]\nsize=2\nstride=2\n\n"
    "[maxpool]\nsize=2\nstride=2\n\n"
    "[convolutional]\nfilters=32\nsize=3\nstride=1\npad=1\nactivation=leaky\n\n"
    "[maxpool]\nsize=2\nstride=2\n\n"
    "[upsample]\nstride=2\n\n"
    "[route]\nlayers=-1,4\n\n"
    "[convolutional]\nfilters=21\nsize=1\nstride=1\nactivation=linear\n\n"
    "[yolo]\nmask=0,1,2\nanchors=10,14,23,27,37,58\nclasses=2\nnum=3\n";

}  // namespace

class CTEST_NetEditorModel : public testing::Test
{
 public:
  void SetUp() override
  {
    ASSERT_TRUE(model.read(YOLO));
    model.classes = {"cat", "dog"};
  }

  /// @brief The row of the option of the layer, -1 for the [net].
  OptionPresenterPtr row(int layer, const std::string& key)
  {
    const auto section = layer < 0
                             ? model.net()
                             : model.layers()[static_cast<std::size_t>(layer)];

    for (const auto& one : OptionPresenter::of(model, section)) {
      if (one->key() == key) {
        return one;
      }
    }

    return nullptr;
  }

  std::string error(int layer, const std::string& key, const std::string& value)
  {
    auto option = row(layer, key);

    option->value(value);

    return option->error();
  }

  NetworkModel model;
};

TEST_F(CTEST_NetEditorModel, the_cfg_is_read_the_darknet_way)
{
  NetworkModel read;

  ASSERT_TRUE(read.read(
      "# comment\n[network]\n batch = 64 \n;comment\nrandom\nbatch=1\n\n"
      "[conv]\nfilters=3\n[route]\nlayers=-1"));

  EXPECT_EQ(read.net()->type(), "network");
  ASSERT_EQ(read.layers().size(), 2U);
  EXPECT_EQ(*read.net()->find("batch"), "64");
  EXPECT_EQ(*read.net()->find("random"), "");
  EXPECT_EQ(read.layers()[1]->integers("layers"), std::vector<int>{-1});

  // the repeated key stays, the darknet takes the first one
  EXPECT_EQ(read.cfg(),
            "[network]\nbatch=64\nrandom\nbatch=1\n\n[conv]\nfilters=3\n\n"
            "[route]\nlayers=-1\n");
  EXPECT_FALSE(read.read("[convolutional]\nfilters=1\n"));
  EXPECT_EQ(read.layers().size(), 2U);
}

TEST_F(CTEST_NetEditorModel, the_cfg_text_goes_back_as_it_was)
{
  EXPECT_EQ(model.cfg(), YOLO);
}

TEST_F(CTEST_NetEditorModel, the_references_follow_the_moved_layers)
{
  // the convolutional 4 before the convolutional 0: the route goes on taking
  // the upsample and the convolutional layer, now the 0 one
  model.move(4, 0);

  EXPECT_EQ(*model.layers()[7]->find("layers"), "-1,0");
  EXPECT_EQ(*model.layers()[0]->find("filters"), "32");

  // the route before the upsample: it's relative layer is the one after it
  // now, which is told by the absolute index
  model.move(7, 6);

  EXPECT_EQ(*model.layers()[6]->find("layers"), "7,0");
  EXPECT_EQ(row(6, "layers")->error(), "The 7 isn't from -6 to 5");
}

TEST_F(CTEST_NetEditorModel, the_references_follow_the_removed_layers)
{
  model.remove(1);

  EXPECT_EQ(model.layers().size(), 9U);
  EXPECT_EQ(*model.layers()[6]->find("layers"), "-1,3");

  // the reference to the removed layer stays
  model.remove(3);

  EXPECT_EQ(*model.layers()[5]->find("layers"), "-1,3");
}

TEST_F(CTEST_NetEditorModel, every_option_of_the_network_is_right)
{
  EXPECT_EQ(NetworkValidator::errors(model), std::vector<std::string>{});
}

TEST_F(CTEST_NetEditorModel, the_numbers_are_checked)
{
  EXPECT_EQ(error(0, "size", "3.5"), "The '3.5' isn't an integer");
  EXPECT_EQ(error(0, "size", "0"), "The 0 isn't 1 or more");
  EXPECT_EQ(error(0, "size", "1,2"), "A single value, not a list");
  EXPECT_EQ(error(-1, "momentum", "1.5"), "The 1.5 isn't from 0 to 1");
  EXPECT_EQ(error(-1, "momentum", ".95"), "");
  EXPECT_EQ(error(-1, "learning_rate", "1e-3"), "");
  EXPECT_EQ(error(-1, "scales", "0.1,"), "The '' isn't a real number");

  EXPECT_TRUE(row(0, "size")->typable("12"));
  EXPECT_FALSE(row(0, "size")->typable("1."));
  EXPECT_TRUE(row(-1, "momentum")->typable("-1.5e+3"));
  EXPECT_FALSE(row(-1, "momentum")->typable("0,5"));
  EXPECT_TRUE(row(-1, "steps")->typable("100,200"));
  EXPECT_FALSE(row(-1, "train")->typable("a b"));
}

TEST_F(CTEST_NetEditorModel, the_limits_follow_the_other_options)
{
  EXPECT_EQ(row(-1, "subdivisions")->description(),
            "The mini batches of the batch: the batch / subdivisions images "
            "go through the network at once, more of them take less memory: "
            "an integer from 1 to 64, a divisor of the batch=64");
  EXPECT_EQ(error(-1, "subdivisions", "24"),
            "The 24 doesn't divide the batch=64");

  // the batch takes the subdivisions of the batch of the moment
  row(-1, "batch")->value("96");

  EXPECT_EQ(row(-1, "subdivisions")->error(), "");
  EXPECT_EQ(error(-1, "burn_in", "7000"), "The 7000 isn't from 0 to 6000");
  EXPECT_EQ(error(-1, "steps", "5400,4800"), "The steps aren't ascending");
  EXPECT_EQ(error(-1, "steps", "4800"),
            "The steps aren't as many as the scales (2)");
  EXPECT_EQ(row(-1, "scales")->error(),
            "The scales aren't as many as the steps (1)");

  // the steps policy requires them
  row(-1, "steps")->value("");

  EXPECT_EQ(row(-1, "steps")->placeholder(), "required");
  EXPECT_EQ(row(-1, "steps")->error(), "The option is required");

  row(-1, "policy")->value("constant");

  EXPECT_EQ(row(-1, "steps")->error(), "");
}

TEST_F(CTEST_NetEditorModel, the_head_takes_the_filters_of_the_classes)
{
  EXPECT_EQ(error(8, "filters", "255"),
            "The next [yolo] layer takes (classes + 5) * masks = (2 + 5) * 3 "
            "= 21 filters");
  EXPECT_EQ(row(8, "filters")->description(),
            "The filters of the layer, the channels of it's output: an "
            "integer 1 or more, the next [yolo] layer takes (classes + 5) * "
            "masks = (2 + 5) * 3 = 21");

  // the classes of the names
  EXPECT_EQ(error(9, "classes", "80"), "The network names 2 classes");

  model.classes.clear();

  EXPECT_EQ(row(9, "classes")->error(), "");
  EXPECT_EQ(row(8, "filters")->error(), "");

  EXPECT_EQ(error(9, "mask", "0,3"), "The 3 isn't from 0 to 2");
  EXPECT_EQ(error(9, "anchors", "10,14"),
            "The anchors aren't the 2 * num = 6 values");
}

TEST_F(CTEST_NetEditorModel, the_layers_refer_to_the_layers_before_them)
{
  EXPECT_EQ(row(7, "layers")->description(),
            "The layers the layer takes the outputs of: comma separated "
            "integers from -7 to 6, the layers before it, counted back for "
            "the negative ones: the layers 6 [upsample], 4 [convolutional]");
  EXPECT_EQ(error(7, "layers", "-1,7"), "The 7 isn't from -7 to 6");
  EXPECT_EQ(error(7, "layers", "-8"), "The -8 isn't from -7 to 6");

  const auto first = model.add("route", {});

  model.move(10, 0);

  EXPECT_EQ(row(0, "layers")->error(), "The option is required");
  EXPECT_EQ(error(0, "layers", "-1"), "The -1 isn't possible");
  EXPECT_EQ(first, model.layers()[0]);
}

TEST_F(CTEST_NetEditorModel, the_input_size_is_the_multiple_of_the_stride)
{
  EXPECT_EQ(error(-1, "width", "408"),
            "The width isn't a multiple of the 16 the layers downsample it "
            "by for the detection layers");
  EXPECT_EQ(error(-1, "width", "400"), "");

  // the network of no detection layer takes any
  model.remove(9);

  EXPECT_EQ(error(-1, "width", "408"), "");
}

TEST_F(CTEST_NetEditorModel, the_rows_are_the_schema_and_the_rest)
{
  model.layers()[1]->set("unknown_key", "5");
  model.layers()[1]->set("stopbackward", "1");

  const auto rows = OptionPresenter::of(model, model.layers()[1]);
  const std::size_t known = SectionSchemas::of("maxpool").options().size();

  ASSERT_EQ(rows.size(), known + 2U);
  EXPECT_EQ(rows.front()->key(), "size");
  EXPECT_EQ(rows[rows.size() - 2U]->key(), "unknown_key");
  EXPECT_EQ(rows.back()->description(),
            "1 stops the backward pass at the layer, the layers before it "
            "aren't trained: an integer 0 or more");
  EXPECT_EQ(rows[rows.size() - 2U]->description(),
            "The option the editor knows nothing of, kept as it is");
  EXPECT_EQ(row(0, "activation")->choices().front(), "leaky");
  EXPECT_EQ(row(0, "dilation")->placeholder(), "default: 1");
  EXPECT_EQ(row(-1, "channels")->placeholder(), "unset");
}

TEST_F(CTEST_NetEditorModel, the_wrong_options_are_named)
{
  row(0, "activation")->value("wrong");
  row(-1, "subdivisions")->value("7");

  EXPECT_EQ(NetworkValidator::errors(model),
            (std::vector<std::string>{
                "[net] subdivisions: The 7 doesn't divide the batch=64",
                "layer 0 [convolutional] activation: The value isn't one of "
                "the 23 values of the list"}));
}

TEST_F(CTEST_NetEditorModel, every_layer_type_is_known_and_valid_once_added)
{
  NetworkModel added;

  ASSERT_TRUE(added.read("[net]\nwidth=416\nheight=416\nchannels=3\n"));

  for (const auto& type : SectionSchemas::layer_types()) {
    const auto& schema = SectionSchemas::of(type);

    EXPECT_FALSE(schema.about().empty()) << type;
    EXPECT_NE(&schema, &SectionSchemas::of("no such type")) << type;
    added.add(type, schema.initial());
  }

  // the references and the heads of the layers before them aside
  for (const auto& error : NetworkValidator::errors(added)) {
    EXPECT_TRUE(error.find("filters") != std::string::npos ||
                error.find("[route]") != std::string::npos ||
                error.find("[shortcut]") != std::string::npos ||
                error.find("from") != std::string::npos ||
                error.find("width") != std::string::npos ||
                error.find("height") != std::string::npos)
        << error;
  }
}
