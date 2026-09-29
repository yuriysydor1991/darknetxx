#include "NetworkChecks.h"

#include <cstdint>
#include <vector>

#include "WeightsNetSpec.h"

namespace weights_factory
{

LAYER_TYPE layer_type(LayerKind kind)
{
  switch (kind) {
    case LayerKind::Convolutional:
      return CONVOLUTIONAL;
    case LayerKind::Connected:
      return CONNECTED;
    case LayerKind::Batchnorm:
      return BATCHNORM;
    case LayerKind::Shortcut:
      return SHORTCUT;
    case LayerKind::Implicit:
      return IMPLICIT;
    case LayerKind::Local:
      return LOCAL;
    case LayerKind::Rnn:
      return RNN;
    case LayerKind::Gru:
      return GRU;
    case LayerKind::Lstm:
      return LSTM;
    case LayerKind::Crnn:
      return CRNN;
    case LayerKind::ConvLstm:
      return CONV_LSTM;
    case LayerKind::Maxpool:
      return MAXPOOL;
    case LayerKind::LocalAvgpool:
      return LOCAL_AVGPOOL;
    case LayerKind::Avgpool:
      return AVGPOOL;
    case LayerKind::Route:
      return ROUTE;
    case LayerKind::Upsample:
      return UPSAMPLE;
    case LayerKind::Reorg:
      return REORG;
    case LayerKind::Dropout:
      return DROPOUT;
    case LayerKind::Activation:
      return ACTIVE;
    case LayerKind::Normalization:
      return NORMALIZATION;
    case LayerKind::Empty:
      return EMPTY;
    case LayerKind::Softmax:
      return SOFTMAX;
    case LayerKind::Yolo:
      return YOLO;
  }

  return BLANK;
}

::testing::AssertionResult matches_spec(const network& net, const NetSpec& spec,
                                        ParseMode mode)
{
  if (net.n != static_cast<int>(spec.layers.size())) {
    return ::testing::AssertionFailure()
           << "the network has " << net.n << " layers instead of "
           << spec.layers.size();
  }

  if (net.batch != spec.mini_batch(mode == ParseMode::Train) ||
      net.subdivisions != spec.subdivisions) {
    return ::testing::AssertionFailure()
           << "the network mini batch " << net.batch << " and subdivisions "
           << net.subdivisions << " instead of "
           << spec.mini_batch(mode == ParseMode::Train) << " and "
           << spec.subdivisions;
  }

  const std::vector<Shape> outputs = spec.outputs();

  for (size_t index = 0U; index < spec.layers.size(); ++index) {
    const layer& l = net.layers[index];
    const Shape& o = outputs[index];

    if (l.type != layer_type(spec.layers[index].kind) || l.out_w != o.w ||
        l.out_h != o.h || l.out_c != o.c ||
        static_cast<uint64_t>(l.outputs) != o.outputs) {
      return ::testing::AssertionFailure()
             << "the layer " << index << " is the darknet type " << l.type
             << " of the " << l.out_w << "x" << l.out_h << "x" << l.out_c
             << " (" << l.outputs << ") output instead of the "
             << to_string(spec.layers[index].kind) << " of the " << o.w << "x"
             << o.h << "x" << o.c << " (" << o.outputs << ") one";
    }
  }

  return ::testing::AssertionSuccess();
}

}  // namespace weights_factory
