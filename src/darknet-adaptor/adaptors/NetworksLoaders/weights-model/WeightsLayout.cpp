#include "src/darknet-adaptor/adaptors/NetworksLoaders/weights-model/WeightsLayout.h"

#include <cstring>
#include <vector>

#include "src/darknet-adaptor/adaptors/NetworksLoaders/weights-model/WeightsSchema.h"
#include "src/darknet.h"

namespace darknet_adaptor::loaders
{

namespace
{

std::size_t count_of(const int value)
{
  return value > 0 ? static_cast<std::size_t>(value) : 0U;
}

/// @brief Collects the entries of the network walk.
class LayoutWalk
{
 public:
  LayoutWalk(WeightsLayoutEntries& entries, const WeightsLayoutOptions& options)
      : mentries{entries}, moptions{options}
  {
  }

  void add(const int index, const LAYER_TYPE type, const WeightsSubLayer sub,
           const WeightsArrayKind kind, float* data, const std::size_t count,
           const int rows = 0, const int cols = 0)
  {
    if (count == 0U) {
      return;
    }

    WeightsLayoutEntry entry;
    entry.layer = index;
    entry.type = type;
    entry.sub = sub;
    entry.kind = kind;
    entry.data = data;
    entry.count = count;
    entry.rows = rows;
    entry.cols = cols;

    mentries.push_back(entry);
  }

  /// @brief The save_convolutional_weights() and the
  /// load_convolutional_weights() arrays, the EMA ones of the
  /// save_convolutional_weights_ema() for the ema.
  void convolutional(const int index, const LAYER_TYPE type,
                     const WeightsSubLayer sub, const layer& l, const bool ema)
  {
    const std::size_t n = count_of(l.n);

    add(index, type, sub, WeightsArrayKind::Biases,
        ema ? l.biases_ema : l.biases, n);

    if (l.batch_normalize && !(moptions.loading && l.dontloadscales)) {
      add(index, type, sub, WeightsArrayKind::Scales,
          ema ? l.scales_ema : l.scales, n);
      add(index, type, sub, WeightsArrayKind::RollingMean, l.rolling_mean, n);
      add(index, type, sub, WeightsArrayKind::RollingVariance,
          l.rolling_variance, n);
    }

    // the loader transposes the kernel x filters matrix of the flipped layers
    const bool flipped = moptions.loading && l.flipped;

    add(index, type, sub, WeightsArrayKind::Weights,
        ema ? l.weights_ema : l.weights, count_of(l.nweights),
        flipped ? (l.c / l.groups) * l.size * l.size : 0, flipped ? l.n : 0);
  }

  /// @brief The save_connected_weights() and the load_connected_weights()
  /// arrays.
  void connected(const int index, const LAYER_TYPE type,
                 const WeightsSubLayer sub, const layer& l)
  {
    const std::size_t outputs = count_of(l.outputs);
    // the loader transposes the inputs x outputs matrix of the old files
    const bool transposed = moptions.loading && moptions.transposed;

    add(index, type, sub, WeightsArrayKind::Biases, l.biases, outputs);
    add(index, type, sub, WeightsArrayKind::Weights, l.weights,
        outputs * count_of(l.inputs), transposed ? l.inputs : 0,
        transposed ? l.outputs : 0);

    if (l.batch_normalize && !(moptions.loading && l.dontloadscales)) {
      add(index, type, sub, WeightsArrayKind::Scales, l.scales, outputs);
      add(index, type, sub, WeightsArrayKind::RollingMean, l.rolling_mean,
          outputs);
      add(index, type, sub, WeightsArrayKind::RollingVariance,
          l.rolling_variance, outputs);
    }
  }

  void convolutional_sub(const int index, const LAYER_TYPE type,
                         const WeightsSubLayer sub, const layer* l)
  {
    convolutional(index, type, sub, *l, false);
  }

  void connected_sub(const int index, const LAYER_TYPE type,
                     const WeightsSubLayer sub, const layer* l)
  {
    connected(index, type, sub, *l);
  }

 private:
  WeightsLayoutEntries& mentries;
  const WeightsLayoutOptions& moptions;
};

}  // namespace

WeightsLayoutEntries WeightsLayout::of(const network& net,
                                       const WeightsLayoutOptions& options)
{
  WeightsLayoutEntries entries;
  LayoutWalk walk{entries, options};

  for (int i = 0; i < net.n && i < options.cutoff; ++i) {
    const layer& l = net.layers[i];
    const LAYER_TYPE type = l.type;

    if (options.loading && l.dontload) continue;

    if (type == CONVOLUTIONAL && l.share_layer == NULL) {
      walk.convolutional(i, type, WeightsSubLayer::None, l, options.ema);
    }
    if (type == SHORTCUT && l.nweights > 0) {
      walk.add(i, type, WeightsSubLayer::None, WeightsArrayKind::Weights,
               l.weights, count_of(l.nweights));
    }
    if (type == IMPLICIT) {
      walk.add(i, type, WeightsSubLayer::None, WeightsArrayKind::Weights,
               l.weights, count_of(l.nweights));
    }
    if (type == CONNECTED) {
      walk.connected(i, type, WeightsSubLayer::None, l);
    }
    if (type == BATCHNORM) {
      // the batchnorm layer arrays are read regardless of the dontloadscales
      const std::size_t c = count_of(l.c);

      walk.add(i, type, WeightsSubLayer::None, WeightsArrayKind::Biases,
               l.biases, c);
      walk.add(i, type, WeightsSubLayer::None, WeightsArrayKind::Scales,
               l.scales, c);
      walk.add(i, type, WeightsSubLayer::None, WeightsArrayKind::RollingMean,
               l.rolling_mean, c);
      walk.add(i, type, WeightsSubLayer::None,
               WeightsArrayKind::RollingVariance, l.rolling_variance, c);
    }
    if (type == RNN) {
      walk.connected_sub(i, type, WeightsSubLayer::InputLayer, l.input_layer);
      walk.connected_sub(i, type, WeightsSubLayer::SelfLayer, l.self_layer);
      walk.connected_sub(i, type, WeightsSubLayer::OutputLayer, l.output_layer);
    }
    if (type == GRU) {
      walk.connected_sub(i, type, WeightsSubLayer::InputZLayer,
                         l.input_z_layer);
      walk.connected_sub(i, type, WeightsSubLayer::InputRLayer,
                         l.input_r_layer);
      walk.connected_sub(i, type, WeightsSubLayer::InputHLayer,
                         l.input_h_layer);
      walk.connected_sub(i, type, WeightsSubLayer::StateZLayer,
                         l.state_z_layer);
      walk.connected_sub(i, type, WeightsSubLayer::StateRLayer,
                         l.state_r_layer);
      walk.connected_sub(i, type, WeightsSubLayer::StateHLayer,
                         l.state_h_layer);
    }
    if (type == LSTM) {
      walk.connected_sub(i, type, WeightsSubLayer::Wf, l.wf);
      walk.connected_sub(i, type, WeightsSubLayer::Wi, l.wi);
      walk.connected_sub(i, type, WeightsSubLayer::Wg, l.wg);
      walk.connected_sub(i, type, WeightsSubLayer::Wo, l.wo);
      walk.connected_sub(i, type, WeightsSubLayer::Uf, l.uf);
      walk.connected_sub(i, type, WeightsSubLayer::Ui, l.ui);
      walk.connected_sub(i, type, WeightsSubLayer::Ug, l.ug);
      walk.connected_sub(i, type, WeightsSubLayer::Uo, l.uo);
    }
    if (type == CONV_LSTM) {
      if (l.peephole) {
        walk.convolutional_sub(i, type, WeightsSubLayer::Vf, l.vf);
        walk.convolutional_sub(i, type, WeightsSubLayer::Vi, l.vi);
        walk.convolutional_sub(i, type, WeightsSubLayer::Vo, l.vo);
      }
      walk.convolutional_sub(i, type, WeightsSubLayer::Wf, l.wf);
      if (!l.bottleneck) {
        walk.convolutional_sub(i, type, WeightsSubLayer::Wi, l.wi);
        walk.convolutional_sub(i, type, WeightsSubLayer::Wg, l.wg);
        walk.convolutional_sub(i, type, WeightsSubLayer::Wo, l.wo);
      }
      walk.convolutional_sub(i, type, WeightsSubLayer::Uf, l.uf);
      walk.convolutional_sub(i, type, WeightsSubLayer::Ui, l.ui);
      walk.convolutional_sub(i, type, WeightsSubLayer::Ug, l.ug);
      walk.convolutional_sub(i, type, WeightsSubLayer::Uo, l.uo);
    }
    if (type == CRNN) {
      walk.convolutional_sub(i, type, WeightsSubLayer::InputLayer,
                             l.input_layer);
      walk.convolutional_sub(i, type, WeightsSubLayer::SelfLayer, l.self_layer);
      walk.convolutional_sub(i, type, WeightsSubLayer::OutputLayer,
                             l.output_layer);
    }
    if (type == LOCAL) {
      const std::size_t locations = count_of(l.out_w) * count_of(l.out_h);
      const std::size_t size = count_of(l.size) * count_of(l.size) *
                               count_of(l.c) * count_of(l.n) * locations;

      walk.add(i, type, WeightsSubLayer::None, WeightsArrayKind::Biases,
               l.biases, count_of(l.outputs));
      walk.add(i, type, WeightsSubLayer::None, WeightsArrayKind::Weights,
               l.weights, size);
    }
  }

  return entries;
}

uint64_t WeightsLayout::floats(const WeightsLayoutEntries& layout)
{
  uint64_t count = 0U;

  for (const WeightsLayoutEntry& entry : layout) {
    count += entry.count;
  }

  return count;
}

void WeightsLayout::transpose_matrix(float* a, const int rows, const int cols)
{
  const std::size_t row_count = count_of(rows);
  const std::size_t col_count = count_of(cols);
  const std::size_t size = row_count * col_count;
  std::vector<float> transpose(size);

  for (std::size_t x = 0; x < row_count; ++x) {
    for (std::size_t y = 0; y < col_count; ++y) {
      transpose[y * row_count + x] = a[x * col_count + y];
    }
  }

  if (size > 0U) {
    std::memcpy(a, transpose.data(), size * sizeof(float));
  }
}

}  // namespace darknet_adaptor::loaders
