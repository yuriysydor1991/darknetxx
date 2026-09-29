#ifndef DARKNETXX_PROJECT_WEIGHTS_FACTORY_NET_SPEC_H
#define DARKNETXX_PROJECT_WEIGHTS_FACTORY_NET_SPEC_H

#include <cstddef>
#include <cstdint>
#include <limits>
#include <string>
#include <vector>

namespace weights_factory
{

/// @brief The way the darknet parser builds the network of the cfg file.
enum class ParseMode
{
  /// @brief The parse_network_cfg() training allocations: the updates, the
  /// deltas and the EMA arrays included (the darknetxx trainer way).
  Train,
  /// @brief The parse_network_cfg_custom(cfg, 1, 1) inference allocations
  /// (the darknetxx detector way).
  Inference
};

/// @brief The darknet cfg sections the weights file tests generate.
enum class LayerKind
{
  Convolutional,
  Connected,
  Batchnorm,
  Shortcut,
  Implicit,
  Local,
  Rnn,
  Gru,
  Lstm,
  Crnn,
  ConvLstm,
  Maxpool,
  LocalAvgpool,
  Avgpool,
  Route,
  Upsample,
  Reorg,
  Dropout,
  Activation,
  Normalization,
  Empty,
  Softmax,
  Yolo
};

/**
 * @brief One cfg section of the generated network. Only the fields of the
 * section kind are rendered into the cfg file.
 */
struct LayerSpec
{
  LayerKind kind{LayerKind::Convolutional};

  /// the [convolutional], [local] and [implicit] filters and the [crnn] and
  /// [conv_lstm] output filters
  int filters{1};
  int size{1};
  int stride{1};
  /// the pad=1 option: the size / 2 padding of the convolutions, the 1 pixel
  /// border of the [local] layer
  bool pad{false};
  int groups{1};
  bool batch_normalize{false};
  std::string activation{"linear"};

  /// the [convolutional] options that don't change the weights file layout
  bool binary{false};
  bool xnor{false};
  /// the [convolutional] weights are loaded transposed
  bool flipped{false};
  /// the relative index of the [convolutional] layer to share the weights
  /// with, 0 for none
  int share_index{0};

  /// the [connected], [rnn], [gru] and [lstm] outputs
  int output{1};
  /// the [rnn] hidden outputs and the [crnn] hidden filters
  int hidden{1};

  /// the relative indices of the [shortcut] from and the [route] layers
  std::vector<int> from;
  std::string weights_type{"none"};
  std::string weights_normalization{"none"};

  /// the [implicit] atoms
  int atoms{1};

  bool peephole{false};
  bool bottleneck{false};

  /// the [yolo] classes and anchors (all of them are masked)
  int classes{1};
  int anchors{1};

  /// the options only the weights loader takes into account
  bool dontload{false};
  bool dontloadscales{false};

  LayerSpec& with_bn(bool value = true);
  LayerSpec& with_pad(bool value);
  LayerSpec& with_groups(int value);
  LayerSpec& with_activation(const std::string& value);
  LayerSpec& with_binary();
  LayerSpec& with_xnor();
  LayerSpec& with_flipped();
  LayerSpec& with_share(int relative_index);
  LayerSpec& with_peephole(bool value = true);
  LayerSpec& with_bottleneck(bool value = true);
  LayerSpec& with_dontload();
  LayerSpec& with_dontloadscales();
};

LayerSpec conv(int filters, int size, int stride = 1);
LayerSpec connected(int output);
LayerSpec batchnorm();
LayerSpec shortcut(const std::vector<int>& from,
                   const std::string& weights_type = "none",
                   const std::string& weights_normalization = "none");
LayerSpec implicit(int filters, int atoms);
LayerSpec local(int filters, int size, int stride, bool pad);
LayerSpec rnn(int hidden, int output);
LayerSpec gru(int output);
LayerSpec lstm(int output);
LayerSpec crnn(int hidden, int output, int size);
LayerSpec conv_lstm(int output, int size);
LayerSpec maxpool(int size, int stride);
LayerSpec local_avgpool(int size, int stride);
LayerSpec avgpool();
LayerSpec route(const std::vector<int>& layers);
LayerSpec upsample(int stride);
LayerSpec reorg(int stride);
LayerSpec dropout();
LayerSpec activation(const std::string& name);
LayerSpec normalization();
LayerSpec empty();
LayerSpec softmax();
LayerSpec yolo(int classes, int anchors);

/**
 * @brief The layer output the way darknet computes it: the out_w, out_h,
 * out_c and outputs fields, which are the input of the next layer.
 */
struct Shape
{
  int w{0};
  int h{0};
  int c{0};
  uint64_t outputs{0U};

  bool image() const;
};

/// @brief The sub-layers of the recurrent layers, stored one after another.
enum class SubLayer : uint8_t
{
  None,
  Input,
  Self,
  Output,
  InputZ,
  InputR,
  InputH,
  StateZ,
  StateR,
  StateH,
  Uf,
  Ui,
  Ug,
  Uo,
  Wf,
  Wi,
  Wg,
  Wo,
  Vf,
  Vi,
  Vo
};

/// @brief The layer float arrays a weights file holds.
enum class ArrayKind : uint8_t
{
  Biases,
  Scales,
  RollingMean,
  RollingVariance,
  Weights,
  BiasesEma,
  ScalesEma,
  WeightsEma
};

/**
 * @brief One float array of the weights file, stored right after the previous
 * one.
 */
struct Block
{
  int layer{0};
  SubLayer sub{SubLayer::None};
  ArrayKind array{ArrayKind::Weights};
  uint64_t count{0U};
  /// the loader transposes the array read as the rows x cols matrix, if not 0
  uint64_t rows{0U};
  uint64_t cols{0U};
};

using Layout = std::vector<Block>;

/**
 * @brief Selects the weights file layout part and flavour.
 */
struct LayoutOptions
{
  /// the layers from the 0 one and up to the cutoff (excluded)
  int cutoff{std::numeric_limits<int>::max()};
  /// the save_weights_upto(..., save_ema = 1) arrays
  bool ema{false};
  /// the arrays the loader reads: no dontload layers, no dontloadscales
  /// batch normalization arrays, the flipped layers transposition
  bool loading{false};
  /// the loader transposes the connected weights (major or minor > 1000)
  bool transposed{false};
};

/**
 * @brief The generated network description. It renders the darknet cfg file
 * and computes the weights file layout of the network independently of the
 * darknet code.
 */
struct NetSpec
{
  std::string name;
  int width{16};
  int height{16};
  int channels{3};
  int batch{1};
  int subdivisions{1};
  std::vector<LayerSpec> layers;

  /// @brief The [net] section input.
  Shape input() const;

  /// @brief The outputs of every layer.
  std::vector<Shape> outputs() const;

  /// @brief The darknet cfg file content.
  std::string cfg() const;

  /// @brief The weights file arrays in the file order.
  Layout layout(const LayoutOptions& options = LayoutOptions{}) const;

  /// @brief The rough count of the floats darknet allocates for the network.
  uint64_t estimated_floats(bool train) const;

  /// @brief The network mini batch the darknet parser computes.
  int mini_batch(bool train) const;

  /**
   * @brief Tells if a layer has the input of the w > 1 and h > 1 (as darknet
   * stores the l.w and l.h fields).
   *
   * The darknet parse_network_cfg_custom() divides by the count of such
   * layers and crashes (SIGFPE) on the networks without them: the 1x1 input
   * networks, the connected, the recurrent and the implicit layers on the
   * network input alone.
   */
  bool has_2d_layer() const;
};

uint64_t count_floats(const Layout& layout);

std::string to_string(LayerKind kind);
std::string to_string(SubLayer sub);
std::string to_string(ArrayKind array);
std::string describe(const Block& block);

}  // namespace weights_factory

#endif  // DARKNETXX_PROJECT_WEIGHTS_FACTORY_NET_SPEC_H
