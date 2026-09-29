#include "WeightsNetSpec.h"

#include <algorithm>
#include <numeric>
#include <sstream>

namespace weights_factory
{

namespace
{

uint64_t u64(int value) { return static_cast<uint64_t>(value); }

int conv_out(int in, int size, int stride, bool pad)
{
  const int padding = pad ? size / 2 : 0;
  return (in + 2 * padding - size) / stride + 1;
}

/// @brief The darknet way of the [route] and [shortcut] layer indices: the
/// negative ones are relative to the current layer.
size_t absolute(size_t index, int layer)
{
  return layer < 0 ? index - static_cast<size_t>(-layer)
                   : static_cast<size_t>(layer);
}

uint64_t volume(const Shape& shape)
{
  return u64(shape.w) * u64(shape.h) * u64(shape.c);
}

std::string join(const std::vector<int>& values)
{
  std::ostringstream joined;

  for (size_t index = 0U; index < values.size(); ++index) {
    joined << (index == 0U ? "" : ",") << values[index];
  }

  return joined.str();
}

/// @brief Appends the arrays save_convolutional_weights*() writes.
void add_conv(Layout& layout, int layer, SubLayer sub, int c, int n, int size,
              int groups, bool bn, bool skip_bn, bool ema, bool flipped)
{
  const uint64_t filter = u64(c / groups) * u64(size) * u64(size);

  layout.push_back(Block{
      layer, sub, ema ? ArrayKind::BiasesEma : ArrayKind::Biases, u64(n)});

  if (bn && !skip_bn) {
    layout.push_back(Block{
        layer, sub, ema ? ArrayKind::ScalesEma : ArrayKind::Scales, u64(n)});
    layout.push_back(Block{layer, sub, ArrayKind::RollingMean, u64(n)});
    layout.push_back(Block{layer, sub, ArrayKind::RollingVariance, u64(n)});
  }

  Block weights{layer, sub, ema ? ArrayKind::WeightsEma : ArrayKind::Weights,
                filter * u64(n)};

  if (flipped) {
    weights.rows = filter;
    weights.cols = u64(n);
  }

  layout.push_back(weights);
}

/// @brief Appends the arrays save_connected_weights() writes.
void add_connected(Layout& layout, int layer, SubLayer sub, uint64_t inputs,
                   int outputs, bool bn, bool skip_bn, bool transposed)
{
  layout.push_back(Block{layer, sub, ArrayKind::Biases, u64(outputs)});

  Block weights{layer, sub, ArrayKind::Weights, inputs * u64(outputs)};

  if (transposed) {
    weights.rows = inputs;
    weights.cols = u64(outputs);
  }

  layout.push_back(weights);

  if (bn && !skip_bn) {
    layout.push_back(Block{layer, sub, ArrayKind::Scales, u64(outputs)});
    layout.push_back(Block{layer, sub, ArrayKind::RollingMean, u64(outputs)});
    layout.push_back(
        Block{layer, sub, ArrayKind::RollingVariance, u64(outputs)});
  }
}

void render(std::ostream& cfg, const LayerSpec& l)
{
  switch (l.kind) {
    case LayerKind::Convolutional:
      cfg << "[convolutional]\nfilters=" << l.filters << "\nsize=" << l.size
          << "\nstride=" << l.stride << "\npad=" << (l.pad ? 1 : 0)
          << "\nactivation=" << l.activation << "\n";
      if (l.groups != 1) cfg << "groups=" << l.groups << "\n";
      if (l.batch_normalize) cfg << "batch_normalize=1\n";
      if (l.binary) cfg << "binary=1\n";
      if (l.xnor) cfg << "xnor=1\n";
      if (l.flipped) cfg << "flipped=1\n";
      if (l.share_index != 0) cfg << "share_index=" << l.share_index << "\n";
      break;
    case LayerKind::Connected:
      cfg << "[connected]\noutput=" << l.output
          << "\nactivation=" << l.activation << "\n";
      if (l.batch_normalize) cfg << "batch_normalize=1\n";
      break;
    case LayerKind::Batchnorm:
      cfg << "[batchnorm]\n";
      break;
    case LayerKind::Shortcut:
      cfg << "[shortcut]\nfrom=" << join(l.from)
          << "\nactivation=" << l.activation << "\n";
      if (l.weights_type != "none") {
        cfg << "weights_type=" << l.weights_type
            << "\nweights_normalization=" << l.weights_normalization << "\n";
      }
      break;
    case LayerKind::Implicit:
      cfg << "[implicit]\nfilters=" << l.filters << "\natoms=" << l.atoms
          << "\nmean=0\nstd=0.2\n";
      break;
    case LayerKind::Local:
      cfg << "[local]\nfilters=" << l.filters << "\nsize=" << l.size
          << "\nstride=" << l.stride << "\npad=" << (l.pad ? 1 : 0)
          << "\nactivation=" << l.activation << "\n";
      break;
    case LayerKind::Rnn:
      cfg << "[rnn]\nhidden=" << l.hidden << "\noutput=" << l.output
          << "\nactivation=" << l.activation << "\n";
      if (l.batch_normalize) cfg << "batch_normalize=1\n";
      break;
    case LayerKind::Gru:
      cfg << "[gru]\noutput=" << l.output << "\n";
      if (l.batch_normalize) cfg << "batch_normalize=1\n";
      break;
    case LayerKind::Lstm:
      cfg << "[lstm]\noutput=" << l.output << "\n";
      if (l.batch_normalize) cfg << "batch_normalize=1\n";
      break;
    case LayerKind::Crnn:
      cfg << "[crnn]\nhidden=" << l.hidden << "\noutput=" << l.filters
          << "\nsize=" << l.size << "\nstride=" << l.stride
          << "\npad=" << (l.pad ? 1 : 0) << "\nactivation=" << l.activation
          << "\n";
      if (l.groups != 1) cfg << "groups=" << l.groups << "\n";
      if (l.batch_normalize) cfg << "batch_normalize=1\n";
      break;
    case LayerKind::ConvLstm:
      cfg << "[conv_lstm]\noutput=" << l.filters << "\nsize=" << l.size
          << "\nstride=" << l.stride << "\npad=" << (l.pad ? 1 : 0)
          << "\nactivation=" << l.activation << "\n";
      if (l.groups != 1) cfg << "groups=" << l.groups << "\n";
      if (l.batch_normalize) cfg << "batch_normalize=1\n";
      if (l.peephole) cfg << "peephole=1\n";
      if (l.bottleneck) cfg << "bottleneck=1\n";
      break;
    case LayerKind::Maxpool:
      cfg << "[maxpool]\nsize=" << l.size << "\nstride=" << l.stride << "\n";
      break;
    case LayerKind::LocalAvgpool:
      cfg << "[local_avgpool]\nsize=" << l.size << "\nstride=" << l.stride
          << "\n";
      break;
    case LayerKind::Avgpool:
      cfg << "[avgpool]\n";
      break;
    case LayerKind::Route:
      cfg << "[route]\nlayers=" << join(l.from) << "\n";
      break;
    case LayerKind::Upsample:
      cfg << "[upsample]\nstride=" << l.stride << "\n";
      break;
    case LayerKind::Reorg:
      cfg << "[reorg3d]\nstride=" << l.stride << "\n";
      break;
    case LayerKind::Dropout:
      cfg << "[dropout]\nprobability=0.1\n";
      break;
    case LayerKind::Activation:
      cfg << "[activation]\nactivation=" << l.activation << "\n";
      break;
    case LayerKind::Normalization:
      cfg << "[lrn]\nsize=5\nalpha=0.0001\nbeta=0.75\nkappa=1\n";
      break;
    case LayerKind::Empty:
      cfg << "[empty]\n";
      break;
    case LayerKind::Softmax:
      cfg << "[softmax]\n";
      break;
    case LayerKind::Yolo: {
      std::vector<int> mask;
      std::vector<int> anchors;

      for (int anchor = 0; anchor < l.anchors; ++anchor) {
        mask.push_back(anchor);
        anchors.push_back(10 + anchor);
        anchors.push_back(14 + anchor);
      }

      cfg << "[yolo]\nmask=" << join(mask) << "\nanchors=" << join(anchors)
          << "\nclasses=" << l.classes << "\nnum=" << l.anchors << "\n";
      break;
    }
  }

  if (l.dontload) cfg << "dontload=1\n";
  if (l.dontloadscales) cfg << "dontloadscales=1\n";
}

}  // namespace

LayerSpec& LayerSpec::with_bn(bool value)
{
  batch_normalize = value;
  return *this;
}

LayerSpec& LayerSpec::with_pad(bool value)
{
  pad = value;
  return *this;
}

LayerSpec& LayerSpec::with_groups(int value)
{
  groups = value;
  return *this;
}

LayerSpec& LayerSpec::with_activation(const std::string& value)
{
  activation = value;
  return *this;
}

LayerSpec& LayerSpec::with_binary()
{
  binary = true;
  return *this;
}

LayerSpec& LayerSpec::with_xnor()
{
  xnor = true;
  return *this;
}

LayerSpec& LayerSpec::with_flipped()
{
  flipped = true;
  return *this;
}

LayerSpec& LayerSpec::with_share(int relative_index)
{
  share_index = relative_index;
  return *this;
}

LayerSpec& LayerSpec::with_peephole(bool value)
{
  peephole = value;
  return *this;
}

LayerSpec& LayerSpec::with_bottleneck(bool value)
{
  bottleneck = value;
  return *this;
}

LayerSpec& LayerSpec::with_dontload()
{
  dontload = true;
  return *this;
}

LayerSpec& LayerSpec::with_dontloadscales()
{
  dontloadscales = true;
  return *this;
}

LayerSpec conv(int filters, int size, int stride)
{
  LayerSpec l;
  l.kind = LayerKind::Convolutional;
  l.filters = filters;
  l.size = size;
  l.stride = stride;
  l.pad = true;
  l.activation = "leaky";
  return l;
}

LayerSpec connected(int output)
{
  LayerSpec l;
  l.kind = LayerKind::Connected;
  l.output = output;
  l.activation = "leaky";
  return l;
}

LayerSpec batchnorm()
{
  LayerSpec l;
  l.kind = LayerKind::Batchnorm;
  return l;
}

LayerSpec shortcut(const std::vector<int>& from,
                   const std::string& weights_type,
                   const std::string& weights_normalization)
{
  LayerSpec l;
  l.kind = LayerKind::Shortcut;
  l.from = from;
  l.weights_type = weights_type;
  l.weights_normalization = weights_normalization;
  return l;
}

LayerSpec implicit(int filters, int atoms)
{
  LayerSpec l;
  l.kind = LayerKind::Implicit;
  l.filters = filters;
  l.atoms = atoms;
  return l;
}

LayerSpec local(int filters, int size, int stride, bool pad)
{
  LayerSpec l;
  l.kind = LayerKind::Local;
  l.filters = filters;
  l.size = size;
  l.stride = stride;
  l.pad = pad;
  l.activation = "leaky";
  return l;
}

LayerSpec rnn(int hidden, int output)
{
  LayerSpec l;
  l.kind = LayerKind::Rnn;
  l.hidden = hidden;
  l.output = output;
  l.activation = "leaky";
  return l;
}

LayerSpec gru(int output)
{
  LayerSpec l;
  l.kind = LayerKind::Gru;
  l.output = output;
  return l;
}

LayerSpec lstm(int output)
{
  LayerSpec l;
  l.kind = LayerKind::Lstm;
  l.output = output;
  return l;
}

LayerSpec crnn(int hidden, int output, int size)
{
  LayerSpec l;
  l.kind = LayerKind::Crnn;
  l.hidden = hidden;
  l.filters = output;
  l.size = size;
  l.pad = true;
  l.activation = "leaky";
  return l;
}

LayerSpec conv_lstm(int output, int size)
{
  LayerSpec l;
  l.kind = LayerKind::ConvLstm;
  l.filters = output;
  l.size = size;
  l.pad = true;
  return l;
}

LayerSpec maxpool(int size, int stride)
{
  LayerSpec l;
  l.kind = LayerKind::Maxpool;
  l.size = size;
  l.stride = stride;
  return l;
}

LayerSpec local_avgpool(int size, int stride)
{
  LayerSpec l;
  l.kind = LayerKind::LocalAvgpool;
  l.size = size;
  l.stride = stride;
  return l;
}

LayerSpec avgpool()
{
  LayerSpec l;
  l.kind = LayerKind::Avgpool;
  return l;
}

LayerSpec route(const std::vector<int>& layers)
{
  LayerSpec l;
  l.kind = LayerKind::Route;
  l.from = layers;
  return l;
}

LayerSpec upsample(int stride)
{
  LayerSpec l;
  l.kind = LayerKind::Upsample;
  l.stride = stride;
  return l;
}

LayerSpec reorg(int stride)
{
  LayerSpec l;
  l.kind = LayerKind::Reorg;
  l.stride = stride;
  return l;
}

LayerSpec dropout()
{
  LayerSpec l;
  l.kind = LayerKind::Dropout;
  return l;
}

LayerSpec activation(const std::string& name)
{
  LayerSpec l;
  l.kind = LayerKind::Activation;
  l.activation = name;
  return l;
}

LayerSpec normalization()
{
  LayerSpec l;
  l.kind = LayerKind::Normalization;
  return l;
}

LayerSpec empty()
{
  LayerSpec l;
  l.kind = LayerKind::Empty;
  return l;
}

LayerSpec softmax()
{
  LayerSpec l;
  l.kind = LayerKind::Softmax;
  return l;
}

LayerSpec yolo(int classes, int anchors)
{
  LayerSpec l;
  l.kind = LayerKind::Yolo;
  l.classes = classes;
  l.anchors = anchors;
  return l;
}

bool Shape::image() const { return w > 0 && h > 0 && c > 0; }

Shape NetSpec::input() const
{
  Shape in{width, height, channels, 0U};
  in.outputs = volume(in);
  return in;
}

std::vector<Shape> NetSpec::outputs() const
{
  std::vector<Shape> out;
  out.reserve(layers.size());

  Shape in = input();

  for (size_t index = 0U; index < layers.size(); ++index) {
    const LayerSpec& l = layers[index];
    Shape o = in;

    switch (l.kind) {
      case LayerKind::Convolutional:
      case LayerKind::Crnn:
      case LayerKind::ConvLstm:
        o.w = conv_out(in.w, l.size, l.stride, l.pad);
        o.h = conv_out(in.h, l.size, l.stride, l.pad);
        o.c = l.filters;
        o.outputs = volume(o);
        break;
      case LayerKind::Local:
        o.w = (in.w - (l.pad ? 1 : l.size)) / l.stride + 1;
        o.h = (in.h - (l.pad ? 1 : l.size)) / l.stride + 1;
        o.c = l.filters;
        o.outputs = volume(o);
        break;
      case LayerKind::Connected:
      case LayerKind::Rnn:
      case LayerKind::Lstm:
        o = Shape{1, 1, l.output, u64(l.output)};
        break;
      case LayerKind::Gru:
        // make_gru_layer() leaves the out_w, out_h and out_c fields zeroed
        o = Shape{0, 0, 0, u64(l.output)};
        break;
      case LayerKind::Implicit:
        o = Shape{1, l.atoms, l.filters, u64(l.atoms) * u64(l.filters)};
        break;
      case LayerKind::Maxpool:
      case LayerKind::LocalAvgpool:
        // the default padding is size - 1
        o.w = (in.w + l.size - 1 - l.size) / l.stride + 1;
        o.h = (in.h + l.size - 1 - l.size) / l.stride + 1;
        o.outputs = volume(o);
        break;
      case LayerKind::Avgpool:
        o = Shape{1, 1, in.c, u64(in.c)};
        break;
      case LayerKind::Route: {
        const Shape& first = out[absolute(index, l.from[0])];
        o = Shape{first.w, first.h, 0, 0U};
        for (const int from : l.from) {
          const Shape& next = out[absolute(index, from)];
          o.c += next.c;
          o.outputs += next.outputs;
        }
        break;
      }
      case LayerKind::Upsample:
        o.w = in.w * l.stride;
        o.h = in.h * l.stride;
        o.outputs = volume(o);
        break;
      case LayerKind::Reorg:
        o.w = in.w / l.stride;
        o.h = in.h / l.stride;
        o.c = in.c * l.stride * l.stride;
        o.outputs = volume(o);
        break;
      case LayerKind::Batchnorm:
      case LayerKind::Shortcut:
      case LayerKind::Normalization:
        o.outputs = volume(o);
        break;
      case LayerKind::Softmax:
        // parse_softmax() leaves the out_w, out_h and out_c fields zeroed
        o = Shape{0, 0, 0, in.outputs};
        break;
      case LayerKind::Dropout:
      case LayerKind::Activation:
      case LayerKind::Empty:
      case LayerKind::Yolo:
        break;
    }

    out.push_back(o);
    in = o;
  }

  return out;
}

std::string NetSpec::cfg() const
{
  std::ostringstream cfg;

  cfg << "# " << name << "\n[net]\nbatch=" << batch
      << "\nsubdivisions=" << subdivisions << "\nwidth=" << width
      << "\nheight=" << height << "\nchannels=" << channels << "\n";

  for (const LayerSpec& l : layers) {
    cfg << "\n";
    render(cfg, l);
  }

  return cfg.str();
}

Layout NetSpec::layout(const LayoutOptions& options) const
{
  Layout blocks;

  const std::vector<Shape> out = outputs();

  for (size_t index = 0U; index < layers.size(); ++index) {
    const int i = static_cast<int>(index);

    if (i >= options.cutoff) {
      break;
    }

    const LayerSpec& l = layers[index];

    if (options.loading && l.dontload) {
      continue;
    }

    const Shape in = index == 0U ? input() : out[index - 1U];
    const Shape& o = out[index];
    const bool skip_bn = options.loading && l.dontloadscales;
    const bool transposed = options.loading && options.transposed;
    const bool bn = l.batch_normalize;

    switch (l.kind) {
      case LayerKind::Convolutional:
        // the layers sharing the weights of another layer have no own ones
        if (l.share_index == 0) {
          add_conv(blocks, i, SubLayer::None, in.c, l.filters, l.size,
                   l.xnor ? 1 : l.groups, bn, skip_bn, options.ema,
                   options.loading && l.flipped);
        }
        break;
      case LayerKind::Connected:
        add_connected(blocks, i, SubLayer::None, in.outputs, l.output, bn,
                      skip_bn, transposed);
        break;
      case LayerKind::Batchnorm:
        blocks.push_back(
            Block{i, SubLayer::None, ArrayKind::Biases, u64(in.c)});
        blocks.push_back(
            Block{i, SubLayer::None, ArrayKind::Scales, u64(in.c)});
        blocks.push_back(
            Block{i, SubLayer::None, ArrayKind::RollingMean, u64(in.c)});
        blocks.push_back(
            Block{i, SubLayer::None, ArrayKind::RollingVariance, u64(in.c)});
        break;
      case LayerKind::Shortcut:
        if (l.weights_type == "per_feature" || l.weights_type == "per_layer") {
          blocks.push_back(Block{i, SubLayer::None, ArrayKind::Weights,
                                 u64(static_cast<int>(l.from.size()) + 1)});
        } else if (l.weights_type == "per_channel") {
          blocks.push_back(
              Block{i, SubLayer::None, ArrayKind::Weights,
                    u64(static_cast<int>(l.from.size()) + 1) * u64(in.c)});
        }
        break;
      case LayerKind::Implicit:
        blocks.push_back(Block{i, SubLayer::None, ArrayKind::Weights,
                               u64(l.filters) * u64(l.atoms)});
        break;
      case LayerKind::Local:
        blocks.push_back(
            Block{i, SubLayer::None, ArrayKind::Biases, volume(o)});
        blocks.push_back(Block{i, SubLayer::None, ArrayKind::Weights,
                               u64(in.c) * u64(l.filters) * u64(l.size) *
                                   u64(l.size) * u64(o.w) * u64(o.h)});
        break;
      case LayerKind::Rnn:
        add_connected(blocks, i, SubLayer::Input, in.outputs, l.hidden, bn,
                      false, transposed);
        add_connected(blocks, i, SubLayer::Self, u64(l.hidden), l.hidden, bn,
                      false, transposed);
        add_connected(blocks, i, SubLayer::Output, u64(l.hidden), l.output, bn,
                      false, transposed);
        break;
      case LayerKind::Gru:
        for (const SubLayer sub :
             {SubLayer::InputZ, SubLayer::InputR, SubLayer::InputH}) {
          add_connected(blocks, i, sub, in.outputs, l.output, bn, false,
                        transposed);
        }
        for (const SubLayer sub :
             {SubLayer::StateZ, SubLayer::StateR, SubLayer::StateH}) {
          add_connected(blocks, i, sub, u64(l.output), l.output, bn, false,
                        transposed);
        }
        break;
      case LayerKind::Lstm:
        for (const SubLayer sub :
             {SubLayer::Wf, SubLayer::Wi, SubLayer::Wg, SubLayer::Wo}) {
          add_connected(blocks, i, sub, u64(l.output), l.output, bn, false,
                        transposed);
        }
        for (const SubLayer sub :
             {SubLayer::Uf, SubLayer::Ui, SubLayer::Ug, SubLayer::Uo}) {
          add_connected(blocks, i, sub, in.outputs, l.output, bn, false,
                        transposed);
        }
        break;
      case LayerKind::Crnn:
        add_conv(blocks, i, SubLayer::Input, in.c, l.hidden, l.size, l.groups,
                 bn, false, false, false);
        add_conv(blocks, i, SubLayer::Self, l.hidden, l.hidden, l.size,
                 l.groups, bn, false, false, false);
        add_conv(blocks, i, SubLayer::Output, l.hidden, l.filters, l.size,
                 l.groups, bn, false, false, false);
        break;
      case LayerKind::ConvLstm:
        if (l.peephole) {
          for (const SubLayer sub :
               {SubLayer::Vf, SubLayer::Vi, SubLayer::Vo}) {
            add_conv(blocks, i, sub, l.filters, l.filters, l.size, l.groups, bn,
                     false, false, false);
          }
        }
        add_conv(blocks, i, SubLayer::Wf,
                 l.bottleneck ? 2 * l.filters : l.filters, l.filters, l.size,
                 l.groups, bn, false, false, false);
        if (!l.bottleneck) {
          for (const SubLayer sub :
               {SubLayer::Wi, SubLayer::Wg, SubLayer::Wo}) {
            add_conv(blocks, i, sub, l.filters, l.filters, l.size, l.groups, bn,
                     false, false, false);
          }
        }
        for (const SubLayer sub :
             {SubLayer::Uf, SubLayer::Ui, SubLayer::Ug, SubLayer::Uo}) {
          add_conv(blocks, i, sub, in.c, l.filters, l.size, l.groups, bn, false,
                   false, false);
        }
        break;
      case LayerKind::Maxpool:
      case LayerKind::LocalAvgpool:
      case LayerKind::Avgpool:
      case LayerKind::Route:
      case LayerKind::Upsample:
      case LayerKind::Reorg:
      case LayerKind::Dropout:
      case LayerKind::Activation:
      case LayerKind::Normalization:
      case LayerKind::Empty:
      case LayerKind::Softmax:
      case LayerKind::Yolo:
        break;
    }
  }

  return blocks;
}

uint64_t NetSpec::estimated_floats(bool train) const
{
  const std::vector<Shape> out = outputs();
  const uint64_t batch_size = u64(std::max(1, mini_batch(train)));
  const uint64_t copies = train ? 3U : 1U;

  uint64_t floats = count_floats(layout()) * (train ? 3U : 2U);
  uint64_t workspace = 0U;

  for (size_t index = 0U; index < layers.size(); ++index) {
    const LayerSpec& l = layers[index];
    const Shape in = index == 0U ? input() : out[index - 1U];
    const Shape& o = out[index];

    // the output, the delta and the batch normalization copies
    floats += std::max(o.outputs, u64(l.output)) * batch_size * copies;

    if (l.kind == LayerKind::Convolutional) {
      workspace = std::max(workspace, u64(o.w) * u64(o.h) * u64(l.size) *
                                          u64(l.size) * u64(in.c));
    } else if (l.kind == LayerKind::Local) {
      // the col_image and the biases updates
      floats += u64(o.w) * u64(o.h) * u64(l.size) * u64(l.size) * u64(in.c) +
                o.outputs;
    } else if (l.kind == LayerKind::Crnn || l.kind == LayerKind::ConvLstm) {
      // the per sub-layer outputs and states
      floats += u64(o.w) * u64(o.h) * u64(std::max(l.hidden, 2 * l.filters)) *
                batch_size * 16U;
    } else if (l.kind == LayerKind::Maxpool ||
               l.kind == LayerKind::LocalAvgpool) {
      // the indexes
      floats += o.outputs * batch_size;
    }
  }

  return floats + workspace;
}

int NetSpec::mini_batch(bool train) const
{
  if (!train) {
    // parse_network_cfg_custom(cfg, 1, 1) forces the single image batch
    return 1;
  }

  return std::max(1, batch / subdivisions);
}

bool NetSpec::has_2d_layer() const
{
  const std::vector<Shape> out = outputs();
  // the l.w and l.h of every layer
  std::vector<std::pair<int, int>> stored;

  for (size_t index = 0U; index < layers.size(); ++index) {
    const Shape in = index == 0U ? input() : out[index - 1U];
    std::pair<int, int> dims{in.w, in.h};

    switch (layers[index].kind) {
      case LayerKind::Connected:
      case LayerKind::Implicit:
        dims = {1, 1};
        break;
      case LayerKind::Rnn:
      case LayerKind::Gru:
      case LayerKind::Lstm:
      case LayerKind::Dropout:
        dims = {0, 0};
        break;
      case LayerKind::Route:
        dims = stored[absolute(index, layers[index].from[0])];
        break;
      default:
        break;
    }

    if (dims.first > 1 && dims.second > 1) {
      return true;
    }

    stored.push_back(dims);
  }

  return false;
}

uint64_t count_floats(const Layout& layout)
{
  return std::accumulate(
      layout.begin(), layout.end(), uint64_t{0U},
      [](uint64_t floats, const Block& block) { return floats + block.count; });
}

std::string to_string(LayerKind kind)
{
  switch (kind) {
    case LayerKind::Convolutional:
      return "conv";
    case LayerKind::Connected:
      return "connected";
    case LayerKind::Batchnorm:
      return "batchnorm";
    case LayerKind::Shortcut:
      return "shortcut";
    case LayerKind::Implicit:
      return "implicit";
    case LayerKind::Local:
      return "local";
    case LayerKind::Rnn:
      return "rnn";
    case LayerKind::Gru:
      return "gru";
    case LayerKind::Lstm:
      return "lstm";
    case LayerKind::Crnn:
      return "crnn";
    case LayerKind::ConvLstm:
      return "conv_lstm";
    case LayerKind::Maxpool:
      return "maxpool";
    case LayerKind::LocalAvgpool:
      return "local_avgpool";
    case LayerKind::Avgpool:
      return "avgpool";
    case LayerKind::Route:
      return "route";
    case LayerKind::Upsample:
      return "upsample";
    case LayerKind::Reorg:
      return "reorg";
    case LayerKind::Dropout:
      return "dropout";
    case LayerKind::Activation:
      return "activation";
    case LayerKind::Normalization:
      return "lrn";
    case LayerKind::Empty:
      return "empty";
    case LayerKind::Softmax:
      return "softmax";
    case LayerKind::Yolo:
      return "yolo";
  }

  return "unknown";
}

std::string to_string(SubLayer sub)
{
  static const char* const names[] = {
      "",        "input",   "self",    "output",  "input_z", "input_r",
      "input_h", "state_z", "state_r", "state_h", "uf",      "ui",
      "ug",      "uo",      "wf",      "wi",      "wg",      "wo",
      "vf",      "vi",      "vo"};

  return names[static_cast<size_t>(sub)];
}

std::string to_string(ArrayKind array)
{
  static const char* const names[] = {
      "biases",  "scales",     "rolling_mean", "rolling_variance",
      "weights", "biases_ema", "scales_ema",   "weights_ema"};

  return names[static_cast<size_t>(array)];
}

std::string describe(const Block& block)
{
  std::ostringstream description;

  description << "layer " << block.layer;

  if (block.sub != SubLayer::None) {
    description << " " << to_string(block.sub);
  }

  description << " " << to_string(block.array) << "[" << block.count << "]";

  return description.str();
}

}  // namespace weights_factory
