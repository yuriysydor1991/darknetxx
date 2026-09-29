#include "WeightsVariants.h"

#include <algorithm>
#include <cstdlib>
#include <iomanip>
#include <random>
#include <sstream>

namespace weights_factory
{

namespace
{

/// @brief The random networks allocation limit, in floats.
constexpr const uint64_t RANDOM_BUDGET_FLOATS = 24U * 1024U * 1024U;

/// @brief The random weights layers limit, in floats.
constexpr const uint64_t RANDOM_LAYER_FLOATS = 2U * 1024U * 1024U;

const std::vector<std::string>& activations()
{
  static const std::vector<std::string> names{
      "linear",  "leaky",    "logistic",
      "relu",    "relu6",    "elu",
      "selu",    "gelu",     "tanh",
      "swish",   "mish",     "hard_mish",
      "loggy",   "ramp",     "plse",
      "hardtan", "lhtan",    "stair",
      "relie",   "revleaky", "normalize_channels"};

  return names;
}

uint64_t fnv1a(const std::string& text)
{
  uint64_t hash = 14695981039346656037ULL;

  for (const char symbol : text) {
    hash ^= static_cast<unsigned char>(symbol);
    hash *= 1099511628211ULL;
  }

  return hash;
}

uint64_t env_or(const char* name, uint64_t fallback)
{
  const char* value = std::getenv(name);

  if (value == nullptr || *value == '\0') {
    return fallback;
  }

  return std::strtoull(value, nullptr, 10);
}

std::string dims(int width, int height, int channels)
{
  return std::to_string(width) + "x" + std::to_string(height) + "x" +
         std::to_string(channels);
}

NetSpec net(int width, int height, int channels,
            const std::vector<LayerSpec>& layers)
{
  NetSpec spec;
  spec.width = width;
  spec.height = height;
  spec.channels = channels;
  spec.layers = layers;
  return spec;
}

/**
 * @brief The network of the given layers after the linear [activation] one,
 * which passes the input through as it is.
 *
 * The darknet parse_network_cfg_custom() divides by the count of the layers
 * of the w > 1 and h > 1 input and crashes (SIGFPE) without any. The
 * connected, the recurrent and the implicit layers keep no such input, so
 * their networks start with the activation layer, which does.
 */
NetSpec flat_net(int width, int height, int channels,
                 const std::vector<LayerSpec>& layers)
{
  std::vector<LayerSpec> all{activation("linear")};
  all.insert(all.end(), layers.begin(), layers.end());
  return net(width, height, channels, all);
}

/// @brief Collects the variants, each network in both parser modes with the
/// next header of the headers list.
class Collector
{
 public:
  explicit Collector(std::vector<WeightsVariant>& variants)
      : mvariants{variants}, mheaders{variants::headers()}
  {
  }

  void both(const std::string& name, const NetSpec& spec)
  {
    one(name + "_train", spec, ParseMode::Train);
    one(name + "_infer", spec, ParseMode::Inference);
  }

  void one(const std::string& name, const NetSpec& spec, ParseMode mode)
  {
    mvariants.push_back(
        variants::make(name, spec, mode, mheaders[mnext++ % mheaders.size()]));
  }

 private:
  std::vector<WeightsVariant>& mvariants;
  std::vector<WeightsHeader> mheaders;
  size_t mnext{0U};
};

void conv_variants(Collector& add)
{
  for (const bool bn : {false, true}) {
    for (const int size : {1, 3, 5}) {
      for (const int stride : {1, 2}) {
        for (const int groups : {1, 2, 4}) {
          add.both(
              std::string{"conv"} + (bn ? "_bn" : "") + "_size" +
                  std::to_string(size) + "_stride" + std::to_string(stride) +
                  "_groups" + std::to_string(groups),
              net(13, 11, 4,
                  {conv(8, size, stride).with_bn(bn).with_groups(groups)}));
        }
      }
    }
  }

  add.both("conv_depthwise_bn",
           net(12, 12, 6, {conv(6, 3).with_groups(6).with_bn()}));
  add.both("conv_no_pad", net(9, 7, 3, {conv(5, 3).with_pad(false)}));
  add.both("conv_size7_stride3", net(20, 20, 3, {conv(4, 7, 3)}));
  add.both("conv_size2_even", net(8, 8, 2, {conv(3, 2), conv(3, 2, 2)}));
  add.both("conv_single_pixel",
           net(2, 2, 3, {avgpool(), conv(7, 1), conv(5, 3)}));
  add.both("conv_many_channels", net(4, 4, 64, {conv(128, 3).with_bn()}));
  add.both("conv_chain", net(16, 16, 3,
                             {conv(4, 3).with_bn(), conv(8, 3, 2),
                              conv(16, 1).with_bn(), conv(2, 3)}));

  const std::vector<std::string>& names = activations();

  for (size_t index = 0U; index < names.size(); ++index) {
    add.one("conv_activation_" + names[index],
            net(8, 8, 3,
                {conv(4, 3)
                     .with_activation(names[index])
                     .with_bn(index % 2U == 1U)}),
            index % 2U == 0U ? ParseMode::Train : ParseMode::Inference);
  }

  for (const bool bn : {false, true}) {
    const std::string suffix = bn ? "_bn" : "";

    add.both("conv_flipped" + suffix,
             net(10, 10, 3,
                 {conv(6, 3).with_flipped().with_bn(bn),
                  conv(4, 1).with_bn(!bn).with_flipped(), conv(3, 3)}));
    add.both(
        "conv_xnor" + suffix,
        net(10, 10, 3,
            {conv(6, 3).with_xnor().with_bn(bn).with_groups(3), conv(4, 3)}));
    add.both("conv_shared" + suffix,
             net(10, 10, 3,
                 {conv(8, 3).with_bn(bn), conv(8, 3).with_bn(bn),
                  conv(8, 3).with_bn(bn).with_share(-1), conv(2, 1)}));
    add.both("conv_shared_far" + suffix,
             net(10, 10, 3,
                 {conv(8, 3).with_bn(bn), conv(8, 3).with_bn(bn),
                  conv(8, 3).with_bn(bn), conv(8, 3).with_bn(bn).with_share(-2),
                  conv(8, 3).with_bn(bn).with_share(-3)}));
    add.both(
        "conv_dontload" + suffix,
        net(10, 10, 3,
            {conv(4, 3).with_bn(bn), conv(6, 3).with_bn(bn).with_dontload(),
             conv(5, 3).with_bn(bn)}));
    add.both("conv_dontloadscales" + suffix,
             net(10, 10, 3,
                 {conv(4, 3).with_bn(bn),
                  conv(6, 3).with_bn(bn).with_dontloadscales(),
                  conv(5, 3).with_bn(bn)}));
  }

  add.both("conv_binary",
           net(10, 10, 3, {conv(6, 3).with_binary(), conv(4, 3)}));
}

void connected_variants(Collector& add)
{
  const std::vector<Resolution> inputs{{2, 2}, {5, 3}, {16, 16}};
  const std::vector<int> channels{1, 2, 3};

  for (size_t input = 0U; input < inputs.size(); ++input) {
    for (const int output : {1, 7, 33}) {
      for (const bool bn : {false, true}) {
        add.both(std::string{"connected"} + (bn ? "_bn" : "") + "_output" +
                     std::to_string(output) + "_" +
                     dims(inputs[input].width, inputs[input].height,
                          channels[input]),
                 flat_net(inputs[input].width, inputs[input].height,
                          channels[input], {connected(output).with_bn(bn)}));
      }
    }
  }

  add.both("connected_chain", net(6, 6, 3,
                                  {conv(4, 3), connected(10).with_bn(),
                                   connected(5), connected(3).with_bn()}));
  add.both("connected_single_input",
           net(2, 2, 1, {avgpool(), connected(3).with_bn()}));
  add.both("connected_bn_dontloadscales",
           flat_net(4, 4, 2,
                    {connected(6).with_bn().with_dontloadscales(),
                     connected(4).with_bn()}));
  add.both("connected_dontload",
           flat_net(4, 4, 2,
                    {connected(6), connected(4).with_bn().with_dontload(),
                     connected(3)}));
}

void batchnorm_variants(Collector& add)
{
  add.both("batchnorm_1channel", net(6, 6, 1, {batchnorm()}));
  add.both("batchnorm_between_conv",
           net(6, 6, 3, {batchnorm(), conv(4, 3), batchnorm()}));
  add.both("batchnorm_after_connected",
           flat_net(3, 3, 2, {connected(9), batchnorm()}));
  add.both("batchnorm_dontloadscales",
           net(6, 6, 3, {batchnorm().with_dontloadscales(), conv(4, 3)}));
  add.both("batchnorm_dontload",
           net(6, 6, 3, {batchnorm().with_dontload(), conv(3, 3)}));
}

void shortcut_variants(Collector& add)
{
  const std::vector<LayerSpec> base{conv(6, 3), conv(6, 3).with_bn(),
                                    conv(6, 1)};

  const auto with = [&base](const LayerSpec& last) {
    std::vector<LayerSpec> layers = base;
    layers.push_back(last);
    layers.push_back(conv(3, 3));
    return net(9, 8, 3, layers);
  };

  add.both("shortcut_none_1from", with(shortcut({-2})));
  add.both("shortcut_none_2from", with(shortcut({-2, -3})));
  add.both("shortcut_per_feature_relu_1from",
           with(shortcut({-2}, "per_feature", "relu")));
  add.both("shortcut_per_feature_softmax_2from",
           with(shortcut({-2, -3}, "per_feature", "softmax")));
  add.both("shortcut_per_layer_avg_relu_1from",
           with(shortcut({-3}, "per_layer", "avg_relu")));
  add.both("shortcut_per_channel_relu_2from",
           with(shortcut({-2, -3}, "per_channel", "relu")));
  add.both("shortcut_per_channel_softmax_1from",
           with(shortcut({-2}, "per_channel", "softmax")));
  add.both("shortcut_per_channel_dontload",
           with(shortcut({-2}, "per_channel", "relu").with_dontload()));
  add.both("shortcut_per_channel_fewer_channels",
           net(9, 8, 3,
               {conv(6, 3), conv(4, 3), shortcut({-2}, "per_channel", "relu"),
                conv(2, 1)}));
}

void implicit_variants(Collector& add)
{
  for (const auto& [filters, atoms] :
       std::vector<std::pair<int, int>>{{1, 1}, {16, 1}, {5, 3}, {128, 2}}) {
    add.both("implicit_filters" + std::to_string(filters) + "_atoms" +
                 std::to_string(atoms),
             flat_net(8, 8, 3, {implicit(filters, atoms)}));
  }

  add.both("implicit_then_conv",
           net(8, 8, 3, {conv(2, 3), implicit(6, 4), conv(3, 1).with_bn()}));
}

void local_variants(Collector& add)
{
  for (const int filters : {1, 3}) {
    for (const int size : {1, 3}) {
      for (const int stride : {1, 2}) {
        for (const bool pad : {false, true}) {
          add.both("local_filters" + std::to_string(filters) + "_size" +
                       std::to_string(size) + "_stride" +
                       std::to_string(stride) + (pad ? "_pad" : "_nopad"),
                   net(9, 7, 2, {local(filters, size, stride, pad)}));
        }
      }
    }
  }

  add.both(
      "local_between_conv",
      net(8, 8, 3, {conv(3, 3).with_bn(), local(2, 3, 1, true), conv(4, 3)}));
}

void recurrent_variants(Collector& add)
{
  for (const bool bn : {false, true}) {
    const std::string suffix = bn ? "_bn" : "";

    for (const int hidden : {1, 6}) {
      for (const int output : {1, 5}) {
        add.both("rnn" + suffix + "_hidden" + std::to_string(hidden) +
                     "_output" + std::to_string(output),
                 flat_net(5, 4, 2, {rnn(hidden, output).with_bn(bn)}));
      }
    }

    for (const int output : {1, 7}) {
      add.both("gru" + suffix + "_output" + std::to_string(output),
               flat_net(4, 4, 2, {gru(output).with_bn(bn)}));
      add.both("lstm" + suffix + "_output" + std::to_string(output),
               flat_net(4, 3, 3, {lstm(output).with_bn(bn)}));
    }

    for (const int size : {1, 3}) {
      for (const auto& [hidden, output] :
           std::vector<std::pair<int, int>>{{1, 1}, {4, 3}}) {
        add.both("crnn" + suffix + "_size" + std::to_string(size) + "_hidden" +
                     std::to_string(hidden) + "_output" +
                     std::to_string(output),
                 net(6, 5, 2, {crnn(hidden, output, size).with_bn(bn)}));
      }

      for (const bool peephole : {false, true}) {
        for (const bool bottleneck : {false, true}) {
          add.both("conv_lstm" + suffix + "_size" + std::to_string(size) +
                       (peephole ? "_peephole" : "") +
                       (bottleneck ? "_bottleneck" : ""),
                   net(5, 5, 2,
                       {conv_lstm(3, size)
                            .with_peephole(peephole)
                            .with_bottleneck(bottleneck)
                            .with_bn(bn)}));
        }
      }
    }
  }

  add.both("rnn_after_conv", net(6, 6, 3, {conv(3, 3), rnn(4, 2)}));
  add.both("gru_then_connected", flat_net(4, 4, 2, {gru(5), connected(3)}));
  add.both("lstm_then_conv",
           flat_net(4, 4, 2, {lstm(4), conv(3, 1).with_bn()}));
  add.both("crnn_groups",
           net(6, 6, 4, {crnn(4, 2, 3).with_groups(2).with_bn()}));
  add.both("conv_lstm_groups",
           net(6, 6, 4, {conv_lstm(4, 3).with_groups(2).with_peephole()}));
}

void passing_variants(Collector& add)
{
  const auto between = [](const LayerSpec& passing) {
    return net(12, 10, 3, {conv(4, 3).with_bn(), passing, conv(5, 3)});
  };

  add.both("pass_maxpool", between(maxpool(2, 2)));
  add.both("pass_maxpool_size3_stride1", between(maxpool(3, 1)));
  add.both("pass_local_avgpool", between(local_avgpool(2, 2)));
  add.both("pass_avgpool", between(avgpool()));
  add.both("pass_upsample", between(upsample(2)));
  add.both("pass_reorg", between(reorg(2)));
  add.both("pass_dropout", between(dropout()));
  add.both("pass_activation", between(activation("mish")));
  add.both("pass_lrn", between(normalization()));
  add.both("pass_empty", between(empty()));
  add.both("pass_route_1layer",
           net(12, 10, 3, {conv(4, 3), conv(5, 3), route({-2}), conv(6, 3)}));
  add.both(
      "pass_route_2layers",
      net(12, 10, 3, {conv(4, 3), conv(5, 3), route({-1, -2}), conv(6, 3)}));
  add.both("pass_softmax", net(12, 10, 3, {conv(4, 3), softmax()}));
  add.both("pass_yolo", net(32, 32, 3, {conv(4, 3), conv(16, 1), yolo(3, 2)}));
}

void everything_variants(Collector& add)
{
  add.both("everything",
           net(32, 32, 3,
               {conv(8, 3).with_bn(), batchnorm(), conv(8, 3).with_bn(),
                shortcut({-3}, "per_channel", "relu"), maxpool(2, 2),
                local(4, 3, 1, true), crnn(4, 4, 3).with_bn(),
                conv_lstm(4, 3).with_peephole().with_bn(), implicit(8, 1),
                conv(6, 1).with_bn(), avgpool(), rnn(8, 6).with_bn(), lstm(5),
                gru(4), connected(3).with_bn(), softmax()}));
}

/// @brief The layer of the random networks.
class RandomNetBuilder
{
 public:
  RandomNetBuilder(std::mt19937_64& rng, NetSpec& spec) : mrng{rng}, mspec{spec}
  {
  }

  int pick(int count)
  {
    return static_cast<int>(mrng() % static_cast<uint64_t>(count));
  }

  bool chance(int percents) { return pick(100) < percents; }

  void add_layer()
  {
    const std::vector<Shape> outputs = mspec.outputs();
    const Shape in = outputs.empty() ? mspec.input() : outputs.back();
    const size_t index = mspec.layers.size();

    if (!in.image()) {
      add_flat_layer(in);
      return;
    }

    const int roll = pick(100);

    if (roll < 28) {
      add_conv(in, outputs);
    } else if (roll < 36) {
      const int size = 2 + pick(2);
      mspec.layers.push_back(maxpool(size, 1 + pick(2)));
    } else if (roll < 38) {
      const int size = 2 + pick(2);
      mspec.layers.push_back(local_avgpool(size, 1 + pick(2)));
    } else if (roll < 40) {
      mspec.layers.push_back(avgpool());
    } else if (roll < 45) {
      add_loader_options(mspec.layers.emplace_back(batchnorm()), false);
    } else if (roll < 51 && index > 0U) {
      add_shortcut(in, outputs);
    } else if (roll < 55 && index > 0U) {
      add_route(outputs);
    } else if (roll < 58 && volume(in) * 4U <= 65536U) {
      mspec.layers.push_back(upsample(2));
    } else if (roll < 61 && in.w % 2 == 0 && in.h % 2 == 0) {
      mspec.layers.push_back(reorg(2));
    } else if (roll < 66) {
      add_local(in);
    } else if (roll < 70) {
      const int size = pick(2) == 0 ? 1 : 3;
      const int hidden = 1 + pick(6);
      LayerSpec l = crnn(hidden, 1 + pick(6), size).with_bn(chance(50));
      add_loader_options(mspec.layers.emplace_back(l), l.batch_normalize);
    } else if (roll < 74) {
      const int size = pick(2) == 0 ? 1 : 3;
      LayerSpec l = conv_lstm(1 + pick(5), size)
                        .with_peephole(chance(50))
                        .with_bottleneck(chance(50))
                        .with_bn(chance(50));
      add_loader_options(mspec.layers.emplace_back(l), l.batch_normalize);
    } else if (roll < 76) {
      const int filters = 1 + pick(12);
      add_loader_options(
          mspec.layers.emplace_back(implicit(filters, 1 + pick(3))), false);
    } else if (roll < 79 && index > 0U) {
      mspec.layers.push_back(dropout());
    } else if (roll < 81) {
      mspec.layers.push_back(activation(random_activation()));
    } else if (roll < 83) {
      mspec.layers.push_back(normalization());
    } else if (roll < 85 && index > 0U) {
      mspec.layers.push_back(empty());
    } else {
      add_flat_layer(in);
    }
  }

  /// @brief The softmax or the yolo layer at the end of the network.
  void add_final_layer()
  {
    const std::vector<Shape> outputs = mspec.outputs();
    const Shape in = outputs.empty() ? mspec.input() : outputs.back();
    const int roll = pick(100);

    if (roll < 15) {
      mspec.layers.push_back(softmax());
    } else if (roll < 25 && in.image()) {
      const int classes = 1 + pick(4);
      const int anchors = 1 + pick(3);

      mspec.layers.push_back(conv(anchors * (classes + 5), 1));
      mspec.layers.push_back(yolo(classes, anchors));
    }
  }

 private:
  static uint64_t volume(const Shape& shape)
  {
    return static_cast<uint64_t>(shape.w) * static_cast<uint64_t>(shape.h) *
           static_cast<uint64_t>(shape.c);
  }

  const std::string& random_activation()
  {
    const std::vector<std::string>& names = activations();
    return names[static_cast<size_t>(pick(static_cast<int>(names.size())))];
  }

  void add_loader_options(LayerSpec& l, bool bn)
  {
    const int roll = pick(100);

    if (roll < 4) {
      l.dontload = true;
    } else if (roll < 8 && bn) {
      l.dontloadscales = true;
    }
  }

  void add_conv(const Shape& in, const std::vector<Shape>& outputs)
  {
    static const int sizes[] = {1, 2, 3, 3, 3, 5, 7};
    static const int strides[] = {1, 1, 1, 2, 2, 3};

    const int filters = pick(10) == 0 ? 64 : 1 + pick(32);
    const int size = sizes[pick(7)];
    const int stride = strides[pick(6)];

    LayerSpec l = conv(filters, size, stride)
                      .with_bn(chance(50))
                      .with_activation(random_activation());

    if (chance(20) && in.w >= l.size && in.h >= l.size) {
      l.pad = false;
    }

    if (chance(30)) {
      std::vector<int> groups;

      for (const int g : {2, 3, 4, in.c}) {
        if (g > 1 && in.c % g == 0 && l.filters % g == 0) {
          groups.push_back(g);
        }
      }

      if (!groups.empty()) {
        l.groups =
            groups[static_cast<size_t>(pick(static_cast<int>(groups.size())))];
      }
    }

    const uint64_t weights = static_cast<uint64_t>(in.c / l.groups) *
                             static_cast<uint64_t>(l.filters) *
                             static_cast<uint64_t>(l.size * l.size);

    if (weights > RANDOM_LAYER_FLOATS) {
      l.filters = 1;
    }

    const int roll = pick(100);

    if (roll < 8) {
      l.flipped = true;
    } else if (roll < 12 && !l.batch_normalize) {
      l.binary = true;
    } else if (roll < 16) {
      l.xnor = true;
    } else if (roll < 30) {
      share(l, in, outputs);
    }

    if (l.share_index == 0) {
      add_loader_options(l, l.batch_normalize);
    }

    mspec.layers.push_back(l);
  }

  /// @brief Makes the layer share the weights of a random earlier compatible
  /// convolutional layer, if any.
  void share(LayerSpec& l, const Shape& in, const std::vector<Shape>& outputs)
  {
    std::vector<size_t> candidates;

    for (size_t index = 0U; index < mspec.layers.size(); ++index) {
      const LayerSpec& other = mspec.layers[index];
      const Shape other_in = index == 0U ? mspec.input() : outputs[index - 1U];

      if (other.kind == LayerKind::Convolutional && other.share_index == 0 &&
          !other.binary && !other.xnor && other_in.c == in.c) {
        candidates.push_back(index);
      }
    }

    if (candidates.empty()) {
      return;
    }

    const size_t index = candidates[static_cast<size_t>(
        pick(static_cast<int>(candidates.size())))];
    const LayerSpec& other = mspec.layers[index];

    l.filters = other.filters;
    l.size = other.size;
    l.groups = other.groups;
    l.batch_normalize = other.batch_normalize;
    l.flipped = false;
    l.share_index =
        static_cast<int>(index) - static_cast<int>(mspec.layers.size());

    if (!l.pad && (in.w < l.size || in.h < l.size)) {
      l.pad = true;
    }
  }

  void add_shortcut(const Shape& in, const std::vector<Shape>& outputs)
  {
    const size_t index = mspec.layers.size();
    std::vector<int> candidates;

    for (size_t other = 0U; other < index; ++other) {
      if (outputs[other].image() && outputs[other].w == in.w &&
          outputs[other].h == in.h) {
        candidates.push_back(static_cast<int>(other) - static_cast<int>(index));
      }
    }

    if (candidates.empty()) {
      add_conv(in, outputs);
      return;
    }

    // one or two of the candidates
    std::vector<int> from{candidates[static_cast<size_t>(
        pick(static_cast<int>(candidates.size())))]};
    const int second = candidates[static_cast<size_t>(
        pick(static_cast<int>(candidates.size())))];

    if (second != from.front() && chance(40)) {
      from.push_back(second);
    }

    static const char* const types[] = {"none", "per_feature", "per_layer",
                                        "per_channel"};
    static const char* const normalizations[] = {"relu", "avg_relu", "softmax"};

    const std::string type = types[pick(4)];

    LayerSpec l =
        shortcut(from, type, type == "none" ? "none" : normalizations[pick(3)]);

    add_loader_options(l, false);

    mspec.layers.push_back(l);
  }

  void add_route(const std::vector<Shape>& outputs)
  {
    const size_t index = mspec.layers.size();
    const size_t first = static_cast<size_t>(pick(static_cast<int>(index)));

    if (!outputs[first].image()) {
      mspec.layers.push_back(dropout());
      return;
    }

    std::vector<int> layers{static_cast<int>(first) - static_cast<int>(index)};

    for (size_t other = 0U; other < index && layers.size() < 3U; ++other) {
      if (other != first && outputs[other].image() &&
          outputs[other].w == outputs[first].w &&
          outputs[other].h == outputs[first].h && chance(30)) {
        layers.push_back(static_cast<int>(other) - static_cast<int>(index));
      }
    }

    mspec.layers.push_back(route(layers));
  }

  void add_local(const Shape& in)
  {
    const int filters = 1 + pick(4);
    const int size = pick(2) == 0 ? 1 : 3;
    const int stride = 1 + pick(2);

    LayerSpec l = local(filters, size, stride, chance(50))
                      .with_activation(random_activation());

    if (!l.pad && (in.w < l.size || in.h < l.size)) {
      l.pad = true;
    }

    const auto out_w =
        static_cast<uint64_t>((in.w - (l.pad ? 1 : l.size)) / l.stride + 1);
    const auto out_h =
        static_cast<uint64_t>((in.h - (l.pad ? 1 : l.size)) / l.stride + 1);

    if (static_cast<uint64_t>(in.c * l.filters * l.size * l.size) * out_w *
            out_h >
        RANDOM_LAYER_FLOATS) {
      mspec.layers.push_back(maxpool(2, 2));
      return;
    }

    add_loader_options(l, false);

    mspec.layers.push_back(l);
  }

  void add_flat_layer(const Shape& in)
  {
    const int roll = pick(100);
    const auto fits = [&in](int outputs) {
      return in.outputs * static_cast<uint64_t>(outputs) <= RANDOM_LAYER_FLOATS;
    };

    LayerSpec l;

    if (roll < 50) {
      l = connected(1 + pick(24)).with_bn(chance(50));
    } else if (roll < 70) {
      const int hidden = 1 + pick(8);
      l = rnn(hidden, 1 + pick(8)).with_bn(chance(30));
      if (!fits(l.hidden)) l = connected(1);
    } else if (roll < 82) {
      l = lstm(1 + pick(8)).with_bn(chance(30));
    } else if (roll < 94) {
      l = gru(1 + pick(8)).with_bn(chance(30));
    } else if (!mspec.layers.empty()) {
      mspec.layers.push_back(roll < 97 ? dropout() : activation("relu"));
      return;
    } else {
      l = connected(1 + pick(4));
    }

    if (!fits(l.output)) {
      l.output = 1;
    }

    add_loader_options(l, l.batch_normalize);

    mspec.layers.push_back(l);
  }

  std::mt19937_64& mrng;
  NetSpec& mspec;
};

NetSpec random_net(std::mt19937_64& rng, bool train)
{
  const std::vector<Resolution> all = variants::resolutions();

  for (;;) {
    NetSpec spec;
    RandomNetBuilder builder{rng, spec};

    // the smaller half of the resolutions mostly
    const int count = static_cast<int>(all.size());
    const Resolution resolution = all[static_cast<size_t>(
        builder.chance(70) ? builder.pick(count / 2) : builder.pick(count))];

    spec.width = resolution.width;
    spec.height = resolution.height;
    spec.channels = 1 + builder.pick(4);

    static const std::pair<int, int> batches[] = {
        {1, 1}, {2, 2}, {4, 2}, {64, 64}};
    const auto& [batch, subdivisions] = batches[builder.pick(4)];
    spec.batch = batch;
    spec.subdivisions = subdivisions;

    // the big inputs get downsampled first
    const int largest = std::max(spec.width, spec.height);

    if (largest > 256) {
      int size = 2;

      while (largest / size > 256) {
        size *= 2;
      }

      spec.layers.push_back(maxpool(size, size));
    }

    const int depth = 1 + builder.pick(10);

    for (int layer = 0; layer < depth; ++layer) {
      builder.add_layer();
    }

    builder.add_final_layer();

    if (spec.has_2d_layer() &&
        spec.estimated_floats(train) <= RANDOM_BUDGET_FLOATS) {
      return spec;
    }
  }
}

}  // namespace

bool WeightsVariant::train() const { return mode == ParseMode::Train; }

// GoogleTest finds it by the argument dependent lookup
// cppcheck-suppress unusedFunction
void PrintTo(const WeightsVariant& variant, std::ostream* os)
{
  *os << variant.name;
}

namespace variants
{

std::vector<Resolution> resolutions()
{
  // no 1x1: darknet crashes on the networks without a layer of the w > 1
  // and h > 1 input (see the NetSpec::has_2d_layer)
  return {{2, 2},       {3, 3},       {7, 5},      {13, 13},    {28, 28},
          {32, 32},     {64, 48},     {96, 96},    {128, 128},  {224, 224},
          {256, 256},   {320, 240},   {416, 416},  {512, 512},  {608, 608},
          {640, 480},   {800, 600},   {1024, 768}, {1280, 720}, {1920, 1080},
          {2560, 1440}, {3840, 2160}, {4096, 2160}};
}

std::vector<WeightsHeader> headers()
{
  return {
      // the current darknet version
      WeightsHeader{0, 2, 5, 0U},
      // the 32 bits seen versions
      WeightsHeader{0, 1, 0, 0U},
      WeightsHeader{0, 0, 0, 0U},
      WeightsHeader{0, 1, 9999, 0U},
      // the 64 bits seen versions
      WeightsHeader{0, 2, 0, 0U},
      WeightsHeader{1, 0, 0, 0U},
      WeightsHeader{0, 5, 3, 0U},
      WeightsHeader{3, 7, 11, 0U},
      // the last not transposing version
      WeightsHeader{1000, 1000, 0, 0U},
      // the transposing versions
      WeightsHeader{1001, 0, 0, 0U},
      WeightsHeader{0, 1001, 0, 0U},
  };
}

WeightsVariant make(const std::string& name, const NetSpec& spec,
                    ParseMode mode, const WeightsHeader& header)
{
  WeightsVariant variant;

  const uint64_t hash = fnv1a(name);

  variant.name = name;
  variant.spec = spec;
  variant.spec.name = name;
  variant.mode = mode;
  variant.header = header;
  variant.header.seen = (hash >> 17U) % 1000000007ULL;
  variant.iteration = static_cast<int>(hash % 100000U) + 1;
  variant.seed = hash;

  return variant;
}

std::vector<WeightsVariant> layers()
{
  std::vector<WeightsVariant> all;
  Collector add{all};

  conv_variants(add);
  connected_variants(add);
  batchnorm_variants(add);
  shortcut_variants(add);
  implicit_variants(add);
  local_variants(add);
  recurrent_variants(add);
  passing_variants(add);
  everything_variants(add);

  return all;
}

std::vector<WeightsVariant> resolutions_sweep()
{
  std::vector<WeightsVariant> all;
  Collector add{all};

  for (const Resolution& r : resolutions()) {
    for (const int channels : {1, 3}) {
      const std::string suffix = "_" + dims(r.width, r.height, channels);
      const int largest = std::max(r.width, r.height);
      // downsamples to the 64 pixels or less
      const int stem = std::max(1, (largest + 63) / 64);

      // the weights of the convolutions don't depend on the resolution
      add.one("stem_conv" + suffix,
              net(r.width, r.height, channels,
                  {maxpool(stem, stem), conv(8, 3).with_bn(),
                   conv(16, 3, 2).with_bn(), conv(16, 1)}),
              ParseMode::Train);
      // the connected weights grow with the resolution
      add.one("classifier" + suffix,
              net(r.width, r.height, channels,
                  {maxpool(16, 16), conv(4, 3).with_bn(),
                   connected(8).with_bn(), softmax()}),
              ParseMode::Inference);
      // the biggest connected weights: every input pixel of every channel
      add.one("full_res_connected" + suffix,
              flat_net(r.width, r.height, channels, {connected(1)}),
              ParseMode::Inference);
      // the local layer weights of every output pixel
      add.one(
          "full_res_local" + suffix,
          net(r.width, r.height, channels, {local(1, 1, 2, false), conv(2, 1)}),
          ParseMode::Inference);
      add.one("recurrent" + suffix,
              net(r.width, r.height, channels,
                  {maxpool(16, 16), rnn(8, 4).with_bn(), lstm(4), gru(3),
                   connected(2)}),
              ParseMode::Train);
      add.one("conv_recurrent" + suffix,
              net(r.width, r.height, channels,
                  {maxpool(32, 32), crnn(4, 3, 3).with_bn(),
                   conv_lstm(3, 3).with_peephole(), conv(2, 1)}),
              ParseMode::Train);
    }
  }

  return all;
}

std::vector<WeightsVariant> random(uint64_t seed, size_t count)
{
  const uint64_t used_seed = env_or("DARKNETXX_WEIGHTS_TESTS_SEED", seed);
  const size_t used_count =
      env_or("DARKNETXX_WEIGHTS_TESTS_RANDOM_COUNT", count);

  std::mt19937_64 rng{used_seed};
  std::vector<WeightsVariant> all;
  const std::vector<WeightsHeader> all_headers = headers();

  for (size_t index = 0U; index < used_count; ++index) {
    const ParseMode mode =
        rng() % 5U < 3U ? ParseMode::Train : ParseMode::Inference;
    const NetSpec spec = random_net(rng, mode == ParseMode::Train);

    std::ostringstream name;
    name << "random_" << std::setw(4) << std::setfill('0') << index << "_"
         << dims(spec.width, spec.height, spec.channels) << "_"
         << spec.layers.size() << "layers_"
         << (mode == ParseMode::Train ? "train" : "infer");

    all.push_back(
        make(name.str(), spec, mode, all_headers[rng() % all_headers.size()]));
  }

  return all;
}

std::vector<WeightsVariant> big()
{
  std::vector<WeightsVariant> all;
  Collector add{all};

  // yolov3-tiny: the 35 MB weights file
  add.one("yolov3_tiny_416x416x3",
          net(416, 416, 3,
              {conv(16, 3).with_bn(),
               maxpool(2, 2),
               conv(32, 3).with_bn(),
               maxpool(2, 2),
               conv(64, 3).with_bn(),
               maxpool(2, 2),
               conv(128, 3).with_bn(),
               maxpool(2, 2),
               conv(256, 3).with_bn(),
               maxpool(2, 2),
               conv(512, 3).with_bn(),
               maxpool(2, 1),
               conv(1024, 3).with_bn(),
               conv(256, 1).with_bn(),
               conv(512, 3).with_bn(),
               conv(255, 1).with_activation("linear"),
               yolo(80, 3),
               route({-4}),
               conv(128, 1).with_bn(),
               upsample(2),
               route({-1, 8}),
               conv(256, 3).with_bn(),
               conv(255, 1).with_activation("linear"),
               yolo(80, 3)}),
          ParseMode::Inference);

  // yolov3 (the darknet53 backbone and the 3 scales head): the 248 MB
  // weights file
  {
    std::vector<LayerSpec> yolov3{conv(32, 3).with_bn()};
    std::vector<int> stage_ends;

    for (const auto& [filters, blocks] : std::vector<std::pair<int, int>>{
             {64, 1}, {128, 2}, {256, 8}, {512, 8}, {1024, 4}}) {
      yolov3.push_back(conv(filters, 3, 2).with_bn());

      for (int block = 0; block < blocks; ++block) {
        yolov3.push_back(conv(filters / 2, 1).with_bn());
        yolov3.push_back(conv(filters, 3).with_bn());
        yolov3.push_back(shortcut({-3}));
      }

      stage_ends.push_back(static_cast<int>(yolov3.size()) - 1);
    }

    for (const int scale : {0, 1, 2}) {
      const int filters = 512 >> scale;

      if (scale > 0) {
        // the 1x1 convolution of the previous scale, upsampled and
        // concatenated with the backbone stage of the same resolution
        yolov3.push_back(route({-4}));
        yolov3.push_back(conv(filters, 1).with_bn());
        yolov3.push_back(upsample(2));
        yolov3.push_back(
            route({-1, stage_ends[static_cast<size_t>(4 - scale)]}));
      }

      for (int pair = 0; pair < 3; ++pair) {
        yolov3.push_back(conv(filters, 1).with_bn());
        yolov3.push_back(conv(filters * 2, 3).with_bn());
      }

      yolov3.push_back(conv(255, 1).with_activation("linear"));
      yolov3.push_back(yolo(80, 3));
    }

    add.one("yolov3_64x64x3", net(64, 64, 3, yolov3), ParseMode::Inference);
  }

  // the 4K detector: the patchifying stem keeps the activations small
  add.one("detector_3840x2160x3",
          net(3840, 2160, 3,
              {conv(16, 8, 8).with_bn(), maxpool(2, 2), conv(32, 3).with_bn(),
               maxpool(2, 2), conv(64, 3).with_bn(), maxpool(2, 2),
               conv(128, 3).with_bn(), maxpool(2, 2), conv(256, 3).with_bn(),
               maxpool(2, 2), conv(512, 3).with_bn(), conv(1024, 3).with_bn(),
               conv(512, 1).with_bn(), conv(1024, 3).with_bn(),
               conv(255, 1).with_activation("linear"), yolo(80, 3)}),
          ParseMode::Inference);

  add.one("connected_head_3840x2160x1",
          net(3840, 2160, 1, {maxpool(4, 4), connected(16).with_bn()}),
          ParseMode::Inference);

  add.one("local_1920x1080x3",
          net(1920, 1080, 3, {local(2, 3, 3, true), conv(4, 3).with_bn()}),
          ParseMode::Inference);

  return all;
}

}  // namespace variants

}  // namespace weights_factory
