#ifndef DARKNET_CXX_PORT_PROJECT_WEIGHTS_SCHEMA_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_WEIGHTS_SCHEMA_CLASS_H

#include <cstdint>
#include <string>
#include <vector>

#include "src/darknet.h"

namespace darknet_adaptor::loaders
{

/// @brief The float arrays the weights files keep of a layer, named after the
/// layer fields.
enum class WeightsArrayKind : uint8_t
{
  Biases,
  Scales,
  RollingMean,
  RollingVariance,
  Weights
};

/// @brief The sub-layers of the recurrent layers, which keep the arrays of
/// the recurrent layer, named after the layer fields.
enum class WeightsSubLayer : uint8_t
{
  None,
  InputLayer,
  SelfLayer,
  OutputLayer,
  InputZLayer,
  InputRLayer,
  InputHLayer,
  StateZLayer,
  StateRLayer,
  StateHLayer,
  Wf,
  Wi,
  Wg,
  Wo,
  Uf,
  Ui,
  Ug,
  Uo,
  Vf,
  Vi,
  Vo
};

/**
 * @brief The format neutral structure of the weights of every darknet layer
 * kind: the arrays every layer type keeps, their order in the original darknet
 * weights file and their names.
 *
 * The weights layers are the ones the original save_weights_upto() writes:
 * - the convolutional ones: the biases, the batch normalization scales,
 *   rolling mean and rolling variance, then the weights;
 * - the connected ones: the biases, the weights, then the batch normalization
 *   arrays;
 * - the batchnorm ones: the biases and the batch normalization arrays;
 * - the shortcut and the implicit ones: the weights;
 * - the local ones: the biases and the weights;
 * - the recurrent ones, the arrays of their sub-layers one after another: the
 *   connected sub-layers of the rnn, gru and lstm layers and the
 *   convolutional sub-layers of the crnn and conv_lstm ones.
 *
 * The names are the layer fields ones and the cfg sections ones (without the
 * brackets) of the layer types.
 */
class WeightsSchema
{
 public:
  /// @brief The name of the array: the biases, the scales, the rolling_mean,
  /// the rolling_variance or the weights.
  static const std::string& name(WeightsArrayKind kind);

  /// @brief The name of the sub-layer (the input_layer, the wf, ...), the
  /// empty one for the WeightsSubLayer::None.
  static const std::string& name(WeightsSubLayer sub);

  /// @brief The cfg section name of the layer type without the brackets (the
  /// convolutional, the maxpool, ...).
  static const std::string& name(LAYER_TYPE type);

  /// @brief The array of the name, false for an unknown name.
  static bool parse(const std::string& name, WeightsArrayKind& kind);

  /// @brief The sub-layer of the name, false for an unknown (or the empty)
  /// name.
  static bool parse(const std::string& name, WeightsSubLayer& sub);

  /// @brief The layer type of the name, false for an unknown name.
  static bool parse(const std::string& name, LAYER_TYPE& type);

  /// @brief The layer type the darknet parses the cfg section of the header
  /// into (the "[conv]" and the "[convolutional]" ones into the
  /// CONVOLUTIONAL, ...), the BLANK one for an unknown header.
  static LAYER_TYPE section_type(const std::string& header);

  /// @brief Tells the layer types, which keep the weights.
  static bool has_weights(LAYER_TYPE type);

  /// @brief The sub-layers of the recurrent layer type in the file order,
  /// empty for the rest of the types.
  static const std::vector<WeightsSubLayer>& sublayers(LAYER_TYPE type);

  /// @brief The type of the sub-layers of the recurrent layer type (the
  /// CONNECTED or the CONVOLUTIONAL one), the type itself for the rest.
  static LAYER_TYPE sublayer_type(LAYER_TYPE type);

  /// @brief The arrays of the layer type (the arrays of every sub-layer of
  /// the recurrent types) in the file order, empty for the layers without the
  /// weights.
  static const std::vector<WeightsArrayKind>& arrays(LAYER_TYPE type);

  /**
   * @brief The position of the array in the file order of the layer arrays.
   *
   * @return The position, -1 for an array the layers of the type have none.
   */
  static int rank(LAYER_TYPE type, WeightsSubLayer sub, WeightsArrayKind kind);

  /// @brief Describes the array for the messages: "the layer 3 input_layer
  /// biases".
  static std::string describe(int layer, WeightsSubLayer sub,
                              WeightsArrayKind kind);
};

}  // namespace darknet_adaptor::loaders

#endif  // DARKNET_CXX_PORT_PROJECT_WEIGHTS_SCHEMA_CLASS_H
