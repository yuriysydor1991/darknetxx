#include "src/NetworksLoaders/network-weights/NetworkWeights.h"

#include <DarknetxxDataDrivers-0.12/DarknetLayerType.h>

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <utility>

#include "src/NetworksLoaders/DataDriversAliases.h"
#include "src/darknet-adaptor/adaptors/ANetworkAdaptor/ANetworkAdaptor.h"
#include "src/darknet-adaptor/adaptors/orig-includes.h"
#include "src/darknet.h"

namespace darknet_adaptor::loaders
{

namespace
{

// The library writes the version of the weights files the darknet sources of
// the project write.
static_assert(dxxdd::WeightsHeader::CURRENT_MAJOR == MAJOR_VERSION &&
                  dxxdd::WeightsHeader::CURRENT_MINOR == MINOR_VERSION &&
                  dxxdd::WeightsHeader::CURRENT_REVISION == PATCH_VERSION,
              "the weights files version of the library is the darknet one");

std::size_t count_of(const int value)
{
  return value > 0 ? static_cast<std::size_t>(value) : 0U;
}

/// @brief Collects the entries of the network walk.
class LayoutWalk
{
 public:
  LayoutWalk(dxxdd::WeightsLayoutEntries& entries,
             const dxxdd::WeightsLayoutOptions& options)
      : mentries{entries}, moptions{options}
  {
  }

  void add(const int index, const LAYER_TYPE type,
           const dxxdd::WeightsSubLayer sub, const dxxdd::WeightsArrayKind kind,
           float* data, const std::size_t count, const int rows = 0,
           const int cols = 0)
  {
    if (count == 0U) {
      return;
    }

    dxxdd::WeightsLayoutEntry entry;
    entry.layer = index;
    entry.type = dxxdd::DarknetLayerType::type_of(type);
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
                     const dxxdd::WeightsSubLayer sub, const layer& l,
                     const bool ema)
  {
    const std::size_t n = count_of(l.n);

    add(index, type, sub, dxxdd::WeightsArrayKind::Biases,
        ema ? l.biases_ema : l.biases, n);

    if (l.batch_normalize && !(moptions.loading && l.dontloadscales)) {
      add(index, type, sub, dxxdd::WeightsArrayKind::Scales,
          ema ? l.scales_ema : l.scales, n);
      add(index, type, sub, dxxdd::WeightsArrayKind::RollingMean,
          l.rolling_mean, n);
      add(index, type, sub, dxxdd::WeightsArrayKind::RollingVariance,
          l.rolling_variance, n);
    }

    // the loader transposes the kernel x filters matrix of the flipped layers
    const bool flipped = moptions.loading && l.flipped;

    add(index, type, sub, dxxdd::WeightsArrayKind::Weights,
        ema ? l.weights_ema : l.weights, count_of(l.nweights),
        flipped ? (l.c / l.groups) * l.size * l.size : 0, flipped ? l.n : 0);
  }

  /// @brief The save_connected_weights() and the load_connected_weights()
  /// arrays.
  void connected(const int index, const LAYER_TYPE type,
                 const dxxdd::WeightsSubLayer sub, const layer& l)
  {
    const std::size_t outputs = count_of(l.outputs);
    // the loader transposes the inputs x outputs matrix of the old files
    const bool transposed = moptions.loading && moptions.transposed;

    add(index, type, sub, dxxdd::WeightsArrayKind::Biases, l.biases, outputs);
    add(index, type, sub, dxxdd::WeightsArrayKind::Weights, l.weights,
        outputs * count_of(l.inputs), transposed ? l.inputs : 0,
        transposed ? l.outputs : 0);

    if (l.batch_normalize && !(moptions.loading && l.dontloadscales)) {
      add(index, type, sub, dxxdd::WeightsArrayKind::Scales, l.scales, outputs);
      add(index, type, sub, dxxdd::WeightsArrayKind::RollingMean,
          l.rolling_mean, outputs);
      add(index, type, sub, dxxdd::WeightsArrayKind::RollingVariance,
          l.rolling_variance, outputs);
    }
  }

  void convolutional_sub(const int index, const LAYER_TYPE type,
                         const dxxdd::WeightsSubLayer sub, const layer* l)
  {
    convolutional(index, type, sub, *l, false);
  }

  void connected_sub(const int index, const LAYER_TYPE type,
                     const dxxdd::WeightsSubLayer sub, const layer* l)
  {
    connected(index, type, sub, *l);
  }

 private:
  dxxdd::WeightsLayoutEntries& mentries;
  const dxxdd::WeightsLayoutOptions& moptions;
};

}  // namespace

NetworkWeights::NetworkWeights(network& net) : mnet{net} {}

NetworkWeights::NetworkWeights(adaptors::ANetworkAdaptorPtr net)
    : madaptor{std::move(net)}, mnet{*madaptor->get()}
{
}

// The library writes nothing through the view of the const network: the
// read() of the documents and the validators take the const view alone.
NetworkWeights::NetworkWeights(const network& net)
    : mnet{const_cast<network&>(net)}
{
}

NetworkWeightsPtr NetworkWeights::create(adaptors::ANetworkAdaptorPtr net)
{
  return std::make_shared<NetworkWeights>(std::move(net));
}

int NetworkWeights::layers() const { return mnet.n; }

dxxdd::LayerType NetworkWeights::type(const int layer) const
{
  if (layer < 0 || layer >= mnet.n) {
    return dxxdd::LayerType::BLANK;
  }

  return dxxdd::DarknetLayerType::type_of(mnet.layers[layer].type);
}

dxxdd::WeightsLayoutEntries NetworkWeights::layout(
    const dxxdd::WeightsLayoutOptions& options) const
{
  return layout_of(mnet, options);
}

int NetworkWeights::classes() const
{
  return mnet.n > 0 ? mnet.layers[mnet.n - 1].classes : 0;
}

std::string NetworkWeights::names() const
{
  return madaptor == nullptr ? std::string{} : madaptor->names();
}

uint64_t NetworkWeights::seen() const { return *mnet.seen; }

void NetworkWeights::loaded_seen(const uint64_t seen)
{
  *mnet.seen = seen;
  *mnet.cur_iteration = get_current_batch(mnet);
}

uint64_t NetworkWeights::saved_seen()
{
  // remove this line, when you will save to weights-file both: seen &
  // cur_iteration
  (*mnet.seen) = static_cast<uint64_t>(get_current_iteration(mnet) *
                                       mnet.batch * mnet.subdivisions);

  return *mnet.seen;
}

dxxdd::WeightsLayoutEntries NetworkWeights::layout_of(
    const network& net, const dxxdd::WeightsLayoutOptions& options)
{
  dxxdd::WeightsLayoutEntries entries;
  LayoutWalk walk{entries, options};

  for (int i = 0; i < net.n && i < options.cutoff; ++i) {
    const layer& l = net.layers[i];
    const LAYER_TYPE type = l.type;

    if (options.loading && l.dontload) continue;

    if (type == CONVOLUTIONAL && l.share_layer == NULL) {
      walk.convolutional(i, type, dxxdd::WeightsSubLayer::None, l, options.ema);
    }
    if (type == SHORTCUT && l.nweights > 0) {
      walk.add(i, type, dxxdd::WeightsSubLayer::None,
               dxxdd::WeightsArrayKind::Weights, l.weights,
               count_of(l.nweights));
    }
    if (type == IMPLICIT) {
      walk.add(i, type, dxxdd::WeightsSubLayer::None,
               dxxdd::WeightsArrayKind::Weights, l.weights,
               count_of(l.nweights));
    }
    if (type == CONNECTED) {
      walk.connected(i, type, dxxdd::WeightsSubLayer::None, l);
    }
    if (type == BATCHNORM) {
      // the batchnorm layer arrays are read regardless of the dontloadscales
      const std::size_t c = count_of(l.c);

      walk.add(i, type, dxxdd::WeightsSubLayer::None,
               dxxdd::WeightsArrayKind::Biases, l.biases, c);
      walk.add(i, type, dxxdd::WeightsSubLayer::None,
               dxxdd::WeightsArrayKind::Scales, l.scales, c);
      walk.add(i, type, dxxdd::WeightsSubLayer::None,
               dxxdd::WeightsArrayKind::RollingMean, l.rolling_mean, c);
      walk.add(i, type, dxxdd::WeightsSubLayer::None,
               dxxdd::WeightsArrayKind::RollingVariance, l.rolling_variance, c);
    }
    if (type == RNN) {
      walk.connected_sub(i, type, dxxdd::WeightsSubLayer::InputLayer,
                         l.input_layer);
      walk.connected_sub(i, type, dxxdd::WeightsSubLayer::SelfLayer,
                         l.self_layer);
      walk.connected_sub(i, type, dxxdd::WeightsSubLayer::OutputLayer,
                         l.output_layer);
    }
    if (type == GRU) {
      walk.connected_sub(i, type, dxxdd::WeightsSubLayer::InputZLayer,
                         l.input_z_layer);
      walk.connected_sub(i, type, dxxdd::WeightsSubLayer::InputRLayer,
                         l.input_r_layer);
      walk.connected_sub(i, type, dxxdd::WeightsSubLayer::InputHLayer,
                         l.input_h_layer);
      walk.connected_sub(i, type, dxxdd::WeightsSubLayer::StateZLayer,
                         l.state_z_layer);
      walk.connected_sub(i, type, dxxdd::WeightsSubLayer::StateRLayer,
                         l.state_r_layer);
      walk.connected_sub(i, type, dxxdd::WeightsSubLayer::StateHLayer,
                         l.state_h_layer);
    }
    if (type == LSTM) {
      walk.connected_sub(i, type, dxxdd::WeightsSubLayer::Wf, l.wf);
      walk.connected_sub(i, type, dxxdd::WeightsSubLayer::Wi, l.wi);
      walk.connected_sub(i, type, dxxdd::WeightsSubLayer::Wg, l.wg);
      walk.connected_sub(i, type, dxxdd::WeightsSubLayer::Wo, l.wo);
      walk.connected_sub(i, type, dxxdd::WeightsSubLayer::Uf, l.uf);
      walk.connected_sub(i, type, dxxdd::WeightsSubLayer::Ui, l.ui);
      walk.connected_sub(i, type, dxxdd::WeightsSubLayer::Ug, l.ug);
      walk.connected_sub(i, type, dxxdd::WeightsSubLayer::Uo, l.uo);
    }
    if (type == CONV_LSTM) {
      if (l.peephole) {
        walk.convolutional_sub(i, type, dxxdd::WeightsSubLayer::Vf, l.vf);
        walk.convolutional_sub(i, type, dxxdd::WeightsSubLayer::Vi, l.vi);
        walk.convolutional_sub(i, type, dxxdd::WeightsSubLayer::Vo, l.vo);
      }
      walk.convolutional_sub(i, type, dxxdd::WeightsSubLayer::Wf, l.wf);
      if (!l.bottleneck) {
        walk.convolutional_sub(i, type, dxxdd::WeightsSubLayer::Wi, l.wi);
        walk.convolutional_sub(i, type, dxxdd::WeightsSubLayer::Wg, l.wg);
        walk.convolutional_sub(i, type, dxxdd::WeightsSubLayer::Wo, l.wo);
      }
      walk.convolutional_sub(i, type, dxxdd::WeightsSubLayer::Uf, l.uf);
      walk.convolutional_sub(i, type, dxxdd::WeightsSubLayer::Ui, l.ui);
      walk.convolutional_sub(i, type, dxxdd::WeightsSubLayer::Ug, l.ug);
      walk.convolutional_sub(i, type, dxxdd::WeightsSubLayer::Uo, l.uo);
    }
    if (type == CRNN) {
      walk.convolutional_sub(i, type, dxxdd::WeightsSubLayer::InputLayer,
                             l.input_layer);
      walk.convolutional_sub(i, type, dxxdd::WeightsSubLayer::SelfLayer,
                             l.self_layer);
      walk.convolutional_sub(i, type, dxxdd::WeightsSubLayer::OutputLayer,
                             l.output_layer);
    }
    if (type == LOCAL) {
      const std::size_t locations = count_of(l.out_w) * count_of(l.out_h);
      const std::size_t size = count_of(l.size) * count_of(l.size) *
                               count_of(l.c) * count_of(l.n) * locations;

      walk.add(i, type, dxxdd::WeightsSubLayer::None,
               dxxdd::WeightsArrayKind::Biases, l.biases, count_of(l.outputs));
      walk.add(i, type, dxxdd::WeightsSubLayer::None,
               dxxdd::WeightsArrayKind::Weights, l.weights, size);
    }
  }

  return entries;
}

}  // namespace darknet_adaptor::loaders
