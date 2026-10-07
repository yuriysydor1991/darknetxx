#include "src/lib/darknet-adaptor/adaptors/Layers/ConvolutionalLayer.h"

#include <cmath>
#include <cstdarg>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

#include "gemm.h"
#include "src/lib/darknet-adaptor/adaptors/ALayerAdaptor/ALayerAdaptor.h"
#include "src/lib/darknet-adaptor/adaptors/Layers/LayersBuffers.h"
#include "src/lib/darknet-adaptor/adaptors/NetworkError.h"
#include "src/lib/darknet-adaptor/adaptors/orig-includes.h"

namespace darknet_adaptor::adaptors::layers
{

namespace
{

using buffers = LayersBuffers;

bool keeps_activation_input(ACTIVATION activation)
{
  return activation == SWISH || activation == MISH || activation == HARD_MISH;
}

/// @brief Appends the printf() formatted text to the text of the buffer.
__attribute__((format(printf, 2, 3))) void append(std::vector<char>& buff,
                                                  const char* fmt, ...)
{
  const size_t used = std::strlen(buff.data());

  va_list args;
  va_start(args, fmt);
  std::vsnprintf(buff.data() + used, buff.size() - used, fmt, args);
  va_end(args);
}

/// @brief The "conv ..." line of the network layers table darknet prints.
void log_layer(const layer& l, int n, int groups, int size, int stride_x,
               int stride_y, int dilation, int w, int h, int c)
{
  // the snprintf() chain of the original, so the table stays the same
  static const size_t BUFF_SIZE = 4096;
  std::vector<char> buff(BUFF_SIZE, '\0');

  if (l.xnor && l.use_bin_output) {
    append(buff, "%s", "convXB ");
  } else if (l.xnor) {
    append(buff, "%s", "convX ");
  } else if (l.share_layer) {
    append(buff, "%s", "convS ");
  } else if (l.assisted_excitation) {
    append(buff, "%s", "convAE");
  } else {
    append(buff, "%s", "conv  ");
  }

  if (groups > 1) {
    append(buff, "%5d/%4d ", n, groups);
  } else {
    append(buff, "%5d      ", n);
  }

  if (stride_x != stride_y) {
    append(buff, "%2dx%2d/%2dx%2d ", size, size, stride_x, stride_y);
  } else if (dilation > 1) {
    append(buff, "%2d x%2d/%2d(%1d)", size, size, stride_x, dilation);
  } else {
    append(buff, "%2d x%2d/%2d   ", size, size, stride_x);
  }

  append(buff, "%4d x%4d x%4d -> %4d x%4d x%4d %5.3f BF", w, h, c, l.out_w,
         l.out_h, l.out_c, static_cast<double>(l.bflops));

  CLOGIM(buff.data());
}

/**
 * @brief The XNOR buffers of the layer input size: the binarized input, the
 * bit packed input of the 32 channels packs and it's transposed bit matrix of
 * the output size aligned rows (l.bit_align).
 */
void allocate_xnor_inputs(ALayerAdaptor& adaptor)
{
  layer& l = *adaptor.get();

  adaptor.binary_input(buffers::floats(buffers::count(l.inputs, l.batch)));

  const int align = 32;
  const int src_align = l.out_h * l.out_w;
  l.bit_align = src_align + (align - src_align % align);

  const size_t new_c = static_cast<size_t>(l.c / 32);
  const size_t in_re_packed_input_size = new_c * buffers::count(l.w, l.h) + 1;
  adaptor.bin_re_packed_input(
      std::vector<uint32_t>(in_re_packed_input_size, 0U));

  const int k = l.size * l.size * l.c;
  const size_t k_aligned =
      static_cast<size_t>(k + (l.lda_align - k % l.lda_align));
  const size_t t_bit_input_size =
      k_aligned * static_cast<size_t>(l.bit_align) / 8;
  adaptor.t_bit_input(buffers::bytes(t_bit_input_size));
}

/// @brief The fixed blur kernel of the antialiasing sub-layer.
void make_blur_kernel(layer& blur, int n, int blur_size)
{
  const int blur_nweights = n * blur_size * blur_size;

  for (int i = 0; i < blur_nweights; i += (blur_size * blur_size)) {
    if (blur_size == 2) {
      blur.weights[i + 0] = 1 / 4.f;
      blur.weights[i + 1] = 1 / 4.f;
      blur.weights[i + 2] = 1 / 4.f;
      blur.weights[i + 3] = 1 / 4.f;
    } else {
      blur.weights[i + 0] = 1 / 16.f;
      blur.weights[i + 1] = 2 / 16.f;
      blur.weights[i + 2] = 1 / 16.f;

      blur.weights[i + 3] = 2 / 16.f;
      blur.weights[i + 4] = 4 / 16.f;
      blur.weights[i + 5] = 2 / 16.f;

      blur.weights[i + 6] = 1 / 16.f;
      blur.weights[i + 7] = 2 / 16.f;
      blur.weights[i + 8] = 1 / 16.f;
    }
  }

  for (int i = 0; i < n; ++i) {
    blur.biases[i] = 0;
  }
}

}  // namespace

ALayerAdaptorPtr ConvolutionalLayer::make(
    int batch, int steps, int h, int w, int c, int n, int groups, int size,
    int stride_x, int stride_y, int dilation, int padding,
    ACTIVATION activation, int batch_normalize, int binary, int xnor, int adam,
    int use_bin_output, int index, int antialiasing,
    const ALayerAdaptorPtr& share_layer, int assisted_excitation, int deform,
    int train)
{
  const int total_batch = batch * steps;

  ALayerAdaptorPtr adaptor = ALayerAdaptor::create();
  layer& l = *adaptor->get();

  l.type = CONVOLUTIONAL;
  l.train = train;

  if (xnor) groups = 1;  // disable groups for XNOR-net
  if (groups < 1) groups = 1;

  const int blur_stride_x = stride_x;
  const int blur_stride_y = stride_y;
  l.antialiasing = antialiasing;
  if (antialiasing) {
    // use stride=1 in host-layer
    stride_x = stride_y = l.stride = l.stride_x = l.stride_y = 1;
  }

  l.wait_stream_id = -1;
  l.deform = deform;
  l.assisted_excitation = assisted_excitation;
  adaptor->share_layer(share_layer);
  l.index = index;
  l.h = h;
  l.w = w;
  l.c = c;
  l.groups = groups;
  l.n = n;
  l.binary = binary;
  l.xnor = xnor;
  l.use_bin_output = use_bin_output;
  l.batch = batch;
  l.steps = steps;
  l.stride = stride_x;
  l.stride_x = stride_x;
  l.stride_y = stride_y;
  l.dilation = dilation;
  l.size = size;
  l.pad = padding;
  l.batch_normalize = batch_normalize;
  l.learning_rate_scale = 1;
  l.nweights = (c / groups) * n * size * size;

  if (share_layer) {
    const layer& shared = *share_layer->get();

    if (l.size != shared.size || l.nweights != shared.nweights ||
        l.c != shared.c || l.n != shared.n) {
      throw NetworkError(
          "Layer size, nweights, channels or filters don't match for the "
          "share_layer");
    }

    adaptor->borrow_weights(shared.weights);
    adaptor->borrow_weight_updates(shared.weight_updates);

    adaptor->borrow_biases(shared.biases);
    adaptor->borrow_bias_updates(shared.bias_updates);
  } else {
    adaptor->weights(buffers::floats(buffers::count(l.nweights)));
    adaptor->biases(buffers::floats(buffers::count(n)));

    if (train) {
      adaptor->weight_updates(buffers::floats(buffers::count(l.nweights)));
      adaptor->bias_updates(buffers::floats(buffers::count(n)));

      if (l.nweights > 0) {
        adaptor->weights_ema(buffers::floats(buffers::count(l.nweights)));
      }
      adaptor->biases_ema(buffers::floats(buffers::count(n)));
    }
  }

  // The original tests the activation before the layer gets it, so the
  // weights always get the random initialization (the shared ones included).
  const float scale =
      static_cast<float>(std::sqrt(2. / (size * size * c / groups)));
  if (l.activation == NORM_CHAN || l.activation == NORM_CHAN_SOFTMAX ||
      l.activation == NORM_CHAN_SOFTMAX_MAXVAL) {
    for (int i = 0; i < l.nweights; ++i) l.weights[i] = 1;
  } else {
    for (int i = 0; i < l.nweights; ++i) {
      l.weights[i] = scale * rand_uniform(-1, 1);
    }
  }

  const int out_h = convolutional_out_height(l);
  const int out_w = convolutional_out_width(l);
  l.out_h = out_h;
  l.out_w = out_w;
  l.out_c = n;
  l.outputs = l.out_h * l.out_w * l.out_c;
  l.inputs = l.w * l.h * l.c;
  l.activation = activation;

  const size_t outputs_size = buffers::count(total_batch, l.outputs);

  adaptor->output(buffers::floats(outputs_size));
  if (train) adaptor->delta(buffers::floats(outputs_size));

  l.forward = forward_convolutional_layer;
  l.backward = backward_convolutional_layer;
  l.update = update_convolutional_layer;

  if (binary) {
    adaptor->binary_weights(buffers::floats(buffers::count(l.nweights)));
    adaptor->cweights(buffers::bytes(buffers::count(l.nweights)));
    adaptor->scales(buffers::floats(buffers::count(n)));
  }

  if (xnor) {
    adaptor->binary_weights(buffers::floats(buffers::count(l.nweights)));
    adaptor->mean_arr(buffers::floats(buffers::count(l.n)));

    l.lda_align = 256;  // AVX2

    allocate_xnor_inputs(*adaptor);
  }

  if (batch_normalize) {
    if (share_layer) {
      const layer& shared = *share_layer->get();

      adaptor->borrow_scales(shared.scales);
      adaptor->borrow_scale_updates(shared.scale_updates);
      adaptor->borrow_mean(shared.mean);
      adaptor->borrow_variance(shared.variance);
      adaptor->borrow_mean_delta(shared.mean_delta);
      adaptor->borrow_variance_delta(shared.variance_delta);
      adaptor->borrow_rolling_mean(shared.rolling_mean);
      adaptor->borrow_rolling_variance(shared.rolling_variance);
    } else {
      adaptor->scales(buffers::floats(buffers::count(n), 1.f));

      if (train) {
        adaptor->scales_ema(buffers::floats(buffers::count(n)));
        adaptor->scale_updates(buffers::floats(buffers::count(n)));

        adaptor->mean(buffers::floats(buffers::count(n)));
        adaptor->variance(buffers::floats(buffers::count(n)));

        adaptor->mean_delta(buffers::floats(buffers::count(n)));
        adaptor->variance_delta(buffers::floats(buffers::count(n)));
      }

      adaptor->rolling_mean(buffers::floats(buffers::count(n)));
      adaptor->rolling_variance(buffers::floats(buffers::count(n)));
    }

    if (train) {
      adaptor->x(buffers::floats(outputs_size));
      adaptor->x_norm(buffers::floats(outputs_size));
    }
  }

  if (keeps_activation_input(l.activation)) {
    adaptor->activation_input(buffers::floats(outputs_size));
  }

  if (adam) {
    l.adam = 1;
    adaptor->m(buffers::floats(buffers::count(l.nweights)));
    adaptor->v(buffers::floats(buffers::count(l.nweights)));
    adaptor->bias_m(buffers::floats(buffers::count(n)));
    adaptor->scale_m(buffers::floats(buffers::count(n)));
    adaptor->bias_v(buffers::floats(buffers::count(n)));
    adaptor->scale_v(buffers::floats(buffers::count(n)));
  }

  l.workspace_size = get_convolutional_workspace_size(l);

  l.bflops =
      static_cast<float>((2.0 * l.nweights * l.out_h * l.out_w) / 1000000000.);
  if (l.xnor) l.bflops = l.bflops / 32;

  log_layer(l, n, groups, size, stride_x, stride_y, dilation, w, h, c);

  if (l.antialiasing) {
    CLOGIM("AA:  ");

    int blur_size = 3;
    int blur_pad = blur_size / 2;
    if (l.antialiasing == 2) {
      blur_size = 2;
      blur_pad = 0;
    }

    adaptor->input_layer(make(batch, steps, out_h, out_w, n, n, n, blur_size,
                              blur_stride_x, blur_stride_y, 1, blur_pad, LINEAR,
                              0, 0, 0, 0, 0, index, 0, nullptr, 0, 0, train));

    make_blur_kernel(*l.input_layer, n, blur_size);

    // the blur sub-layer computes in the workspace of the layer as well,
    // which the original sized for the layer itself only
    if (l.workspace_size < l.input_layer->workspace_size) {
      l.workspace_size = l.input_layer->workspace_size;
    }
  }

  return adaptor;
}

void ConvolutionalLayer::resize(ALayerAdaptor& adaptor, int w, int h)
{
  layer& l = *adaptor.get();

  const int total_batch = l.batch * l.steps;
  l.w = w;
  l.h = h;
  const int out_w = convolutional_out_width(l);
  const int out_h = convolutional_out_height(l);

  l.out_w = out_w;
  l.out_h = out_h;

  l.outputs = l.out_h * l.out_w * l.out_c;
  l.inputs = l.w * l.h * l.c;

  const size_t outputs_size = buffers::count(total_batch, l.outputs);

  adaptor.output(buffers::resized(adaptor.output(), outputs_size));

  if (l.train) {
    adaptor.delta(buffers::resized(adaptor.delta(), outputs_size));

    if (l.batch_normalize) {
      adaptor.x(buffers::resized(adaptor.x(), outputs_size));
      adaptor.x_norm(buffers::resized(adaptor.x_norm(), outputs_size));
    }
  }

  // The original kept the XNOR input buffers of the initial size (and the
  // bit_align of the initial output), which the forward of the bigger input
  // overflowed.
  if (l.xnor) {
    allocate_xnor_inputs(adaptor);
  }

  if (keeps_activation_input(l.activation)) {
    adaptor.activation_input(
        buffers::resized(adaptor.activation_input(), outputs_size));
  }

  l.workspace_size = get_convolutional_workspace_size(l);

  // The original kept the blur sub-layer of the initial size, which the
  // forward copied into the resized output (past it's end for the smaller
  // one) and the next layer took the output of the layer itself instead.
  if (l.antialiasing && adaptor.input_layer()) {
    ALayerAdaptor& blur = *adaptor.input_layer();

    resize(blur, l.out_w, l.out_h);

    if (l.workspace_size < blur.get()->workspace_size) {
      l.workspace_size = blur.get()->workspace_size;
    }
  }
}

void ConvolutionalLayer::free_batchnorm(ALayerAdaptor& adaptor)
{
  layer& l = *adaptor.get();

  if (l.share_layer != nullptr) {
    // the buffers belong to the shared layer, which releases them
    adaptor.borrow_scales(nullptr);
    adaptor.borrow_scale_updates(nullptr);
    adaptor.borrow_mean(nullptr);
    adaptor.borrow_variance(nullptr);
    adaptor.borrow_mean_delta(nullptr);
    adaptor.borrow_variance_delta(nullptr);
    adaptor.borrow_rolling_mean(nullptr);
    adaptor.borrow_rolling_variance(nullptr);
    return;
  }

  adaptor.scales(std::vector<float>{});
  adaptor.scale_updates(std::vector<float>{});
  adaptor.mean(std::vector<float>{});
  adaptor.variance(std::vector<float>{});
  adaptor.mean_delta(std::vector<float>{});
  adaptor.variance_delta(std::vector<float>{});
  adaptor.rolling_mean(std::vector<float>{});
  adaptor.rolling_variance(std::vector<float>{});
  adaptor.x(std::vector<float>{});
  adaptor.x_norm(std::vector<float>{});
}

void ConvolutionalLayer::binary_align_weights(ALayerAdaptor& adaptor)
{
  layer& l = *adaptor.get();

  const int m = l.n;
  const int k = l.size * l.size * l.c;
  const size_t new_lda =
      static_cast<size_t>(k + (l.lda_align - k % l.lda_align));
  l.new_lda = static_cast<int>(new_lda);

  binarize_weights(l.weights, m, k, l.binary_weights);

  const size_t align_weights_size = new_lda * static_cast<size_t>(m);
  l.align_bit_weights_size = static_cast<int>(align_weights_size / 8 + 1);

  std::vector<float> align_weights = buffers::floats(align_weights_size);
  std::string align_bit_weights =
      buffers::bytes(static_cast<size_t>(l.align_bit_weights_size));

  // align A without transpose
  for (size_t i = 0; i < static_cast<size_t>(m); ++i) {
    for (size_t j = 0; j < static_cast<size_t>(k); ++j) {
      align_weights[i * new_lda + j] =
          l.binary_weights[i * static_cast<size_t>(k) + j];
    }
  }

  if (l.c % 32 == 0) {
    const int items_per_filter = l.c * l.size * l.size;

    for (int fil = 0; fil < l.n; ++fil) {
      for (int chan = 0; chan < l.c; chan += 32) {
        const int items_per_channel = l.size * l.size;

        for (int i = 0; i < items_per_channel; ++i) {
          for (int c_pack = 0; c_pack < 32; ++c_pack) {
            const float src =
                l.binary_weights[fil * items_per_filter +
                                 (chan + c_pack) * items_per_channel + i];

            align_weights[static_cast<size_t>(fil) * new_lda +
                          static_cast<size_t>(chan * items_per_channel +
                                              i * 32 + c_pack)] = src;
          }
        }
      }
    }

    float_to_bit(align_weights.data(),
                 reinterpret_cast<unsigned char*>(align_bit_weights.data()),
                 align_weights_size);

    if (gpu_index >= 0) {
      for (size_t i = 0; i < align_weights_size / 8; ++i) {
        align_bit_weights[i] = static_cast<char>(~(align_bit_weights[i]));
      }
    }
  } else {
    float_to_bit(align_weights.data(),
                 reinterpret_cast<unsigned char*>(align_bit_weights.data()),
                 align_weights_size);
  }

  get_mean_array(l.binary_weights,
                 static_cast<size_t>(m) * static_cast<size_t>(k),
                 static_cast<size_t>(l.n), l.mean_arr);

  adaptor.align_bit_weights(std::move(align_bit_weights));
}

}  // namespace darknet_adaptor::adaptors::layers
