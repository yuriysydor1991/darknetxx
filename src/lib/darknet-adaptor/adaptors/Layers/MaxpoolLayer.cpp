#include "src/lib/darknet-adaptor/adaptors/Layers/MaxpoolLayer.h"

#include "src/lib/darknet-adaptor/adaptors/ALayerAdaptor/ALayerAdaptor.h"
#include "src/lib/darknet-adaptor/adaptors/Layers/ConvolutionalLayer.h"
#include "src/lib/darknet-adaptor/adaptors/Layers/LayersBuffers.h"
#include "src/lib/darknet-adaptor/adaptors/orig-includes.h"

namespace darknet_adaptor::adaptors::layers
{

namespace
{

using buffers = LayersBuffers;

void log_layer(const layer& l, int size, int stride_x, int stride_y,
               int maxpool_depth, int avgpool, int w, int h, int c)
{
  const double bflops = l.bflops;

  if (avgpool) {
    if (stride_x == stride_y) {
      CLOGI(
          "avg               %2dx%2d/%2d   %4d x%4d x%4d -> %4d x%4d x%4d "
          "%5.3f BF",
          size, size, stride_x, w, h, c, l.out_w, l.out_h, l.out_c, bflops);
    } else {
      CLOGI(
          "avg              %2dx%2d/%2dx%2d %4d x%4d x%4d -> %4d x%4d x%4d "
          "%5.3f BF",
          size, size, stride_x, stride_y, w, h, c, l.out_w, l.out_h, l.out_c,
          bflops);
    }
  } else {
    if (maxpool_depth) {
      CLOGI(
          "max-depth         %2dx%2d/%2d   %4d x%4d x%4d -> %4d x%4d x%4d "
          "%5.3f BF",
          size, size, stride_x, w, h, c, l.out_w, l.out_h, l.out_c, bflops);
    } else if (stride_x == stride_y) {
      CLOGI(
          "max               %2dx%2d/%2d   %4d x%4d x%4d -> %4d x%4d x%4d "
          "%5.3f BF",
          size, size, stride_x, w, h, c, l.out_w, l.out_h, l.out_c, bflops);
    } else {
      CLOGI(
          "max              %2dx%2d/%2dx%2d %4d x%4d x%4d -> %4d x%4d x%4d "
          "%5.3f BF",
          size, size, stride_x, stride_y, w, h, c, l.out_w, l.out_h, l.out_c,
          bflops);
    }
  }
}

}  // namespace

ALayerAdaptorPtr MaxpoolLayer::make(int batch, int h, int w, int c, int size,
                                    int stride_x, int stride_y, int padding,
                                    int maxpool_depth, int out_channels,
                                    int antialiasing, int avgpool, int train)
{
  ALayerAdaptorPtr adaptor = ALayerAdaptor::create();
  layer& l = *adaptor->get();

  l.avgpool = avgpool;
  if (avgpool) {
    l.type = LOCAL_AVGPOOL;
  } else {
    l.type = MAXPOOL;
  }
  l.train = train;

  const int blur_stride_x = stride_x;
  const int blur_stride_y = stride_y;
  l.antialiasing = antialiasing;
  if (antialiasing) {
    // use stride=1 in host-layer
    stride_x = stride_y = l.stride = l.stride_x = l.stride_y = 1;
  }

  l.batch = batch;
  l.h = h;
  l.w = w;
  l.c = c;
  l.pad = padding;
  l.maxpool_depth = maxpool_depth;
  l.out_channels = out_channels;
  if (maxpool_depth) {
    l.out_c = out_channels;
    l.out_w = l.w;
    l.out_h = l.h;
  } else {
    l.out_w = (w + padding - size) / stride_x + 1;
    l.out_h = (h + padding - size) / stride_y + 1;
    l.out_c = c;
  }
  l.outputs = l.out_h * l.out_w * l.out_c;
  l.inputs = h * w * c;
  l.size = size;
  l.stride = stride_x;
  l.stride_x = stride_x;
  l.stride_y = stride_y;

  const size_t output_size =
      buffers::count(l.out_h, l.out_w) * buffers::count(l.out_c, batch);

  if (train) {
    if (!avgpool) adaptor->indexes(buffers::ints(output_size));
    adaptor->delta(buffers::floats(output_size));
  }
  adaptor->output(buffers::floats(output_size));

  if (avgpool) {
    l.forward = forward_local_avgpool_layer;
    l.backward = backward_local_avgpool_layer;
  } else {
    l.forward = forward_maxpool_layer;
    l.backward = backward_maxpool_layer;
  }

  l.bflops = static_cast<float>((l.size * l.size * l.c * l.out_h * l.out_w) /
                                1000000000.);

  log_layer(l, size, stride_x, stride_y, maxpool_depth, avgpool, w, h, c);

  if (l.antialiasing) {
    CLOGIM("AA:  ");

    int blur_size = 3;
    int blur_pad = blur_size / 2;
    if (l.antialiasing == 2) {
      blur_size = 2;
      blur_pad = 0;
    }

    adaptor->input_layer(ConvolutionalLayer::make(
        batch, 1, l.out_h, l.out_w, l.out_c, l.out_c, l.out_c, blur_size,
        blur_stride_x, blur_stride_y, 1, blur_pad, LINEAR, 0, 0, 0, 0, 0, 1, 0,
        nullptr, 0, 0, train));

    layer& blur = *l.input_layer;
    const int blur_nweights = l.out_c * blur_size * blur_size;

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

    for (int i = 0; i < l.out_c; ++i) blur.biases[i] = 0;

    // the blur sub-layer computes in the network workspace, which the
    // original didn't size for it
    l.workspace_size = blur.workspace_size;
  }

  return adaptor;
}

void MaxpoolLayer::resize(ALayerAdaptor& adaptor, int w, int h)
{
  layer& l = *adaptor.get();

  l.h = h;
  l.w = w;
  l.inputs = h * w * l.c;

  l.out_w = (w + l.pad - l.size) / l.stride_x + 1;
  l.out_h = (h + l.pad - l.size) / l.stride_y + 1;
  l.outputs = l.out_w * l.out_h * l.out_c;

  const size_t output_size = buffers::count(l.outputs, l.batch);

  if (l.train) {
    if (!l.avgpool) {
      adaptor.indexes(buffers::resized(adaptor.indexes(), output_size));
    }
    adaptor.delta(buffers::resized(adaptor.delta(), output_size));
  }
  adaptor.output(buffers::resized(adaptor.output(), output_size));

  // The original kept the blur sub-layer of the initial size, which the
  // forward copied into the resized output (past it's end for the smaller
  // one) and the next layer took the output of the layer itself instead.
  if (l.antialiasing && adaptor.input_layer()) {
    ALayerAdaptor& blur = *adaptor.input_layer();

    ConvolutionalLayer::resize(blur, l.out_w, l.out_h);

    l.workspace_size = blur.get()->workspace_size;
  }
}

}  // namespace darknet_adaptor::adaptors::layers
