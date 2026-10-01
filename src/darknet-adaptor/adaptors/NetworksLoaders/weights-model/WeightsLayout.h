#ifndef DARKNET_CXX_PORT_PROJECT_WEIGHTS_LAYOUT_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_WEIGHTS_LAYOUT_CLASS_H

#include <cstddef>
#include <cstdint>
#include <limits>
#include <vector>

#include "src/darknet-adaptor/adaptors/NetworksLoaders/weights-model/WeightsSchema.h"
#include "src/darknet.h"

namespace darknet_adaptor::loaders
{

/// @brief Selects the part and the flavour of the network weights layout.
struct WeightsLayoutOptions
{
  /// the layers from the 0 one and up to the cutoff (excluded)
  int cutoff{std::numeric_limits<int>::max()};
  /// the save_weights_upto(..., save_ema = 1) arrays: the EMA biases, scales
  /// and weights of the convolutional layers
  bool ema{false};
  /// the arrays the loaders read: no dontload layers, no batch normalization
  /// arrays of the dontloadscales layers, the flipped convolutional weights
  /// transposed
  bool loading{false};
  /// the loaders transpose the connected weights (the WeightsHeader of the
  /// major or minor > 1000 version)
  bool transposed{false};
};

/// @brief One float array of the network in the order of the weights file.
struct WeightsLayoutEntry
{
  int layer{0};
  /// the type of the network layer
  LAYER_TYPE type{BLANK};
  WeightsSubLayer sub{WeightsSubLayer::None};
  WeightsArrayKind kind{WeightsArrayKind::Weights};
  /**
   * @brief The network array: the EMA one of the EMA arrays. The nullptr is
   * the array the network doesn't allocate (the EMA arrays of the inference
   * networks, for example), which the savers write as the zeros and the
   * loaders skip.
   */
  float* data{nullptr};
  std::size_t count{0U};
  /// the loaders transpose the array read as the rows x cols row major
  /// matrix (the flipped convolutional and the transposed connected
  /// weights), 0 for none
  int rows{0};
  int cols{0};
};

using WeightsLayoutEntries = std::vector<WeightsLayoutEntry>;

/**
 * @brief The float arrays of the network in the order of the weights file:
 * the arrays the original save_weights_upto() wrote and the
 * load_weights_upto() read, the conditions of them included (the layers
 * sharing the weights, the shortcut layers without the weights, the
 * peephole and the bottleneck conv_lstm layers and the rest).
 *
 * The OrigWeightsReader and the OrigWeightsWriter walk the network layers
 * themselves, the layout is the same walk for the rest: the conversion of the
 * original weights files into the documents and back, the dxxwjz1 files and
 * the validators.
 */
class WeightsLayout
{
 public:
  /// @brief The arrays of the network, the empty ones left out.
  static WeightsLayoutEntries of(
      const network& net,
      const WeightsLayoutOptions& options = WeightsLayoutOptions{});

  /// @brief The count of the floats of every entry.
  static uint64_t floats(const WeightsLayoutEntries& layout);

  /// @brief Transposes the row major rows x cols matrix in place.
  static void transpose_matrix(float* a, int rows, int cols);
};

}  // namespace darknet_adaptor::loaders

#endif  // DARKNET_CXX_PORT_PROJECT_WEIGHTS_LAYOUT_CLASS_H
