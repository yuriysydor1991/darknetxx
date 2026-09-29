#include "src/darknet-adaptor/adaptors/Layers/LocalLayer.h"

#include <algorithm>
#include <cmath>

#include "src/darknet-adaptor/adaptors/ALayerAdaptor/ALayerAdaptor.h"
#include "src/darknet-adaptor/adaptors/Layers/LayersBuffers.h"
#include "src/darknet-adaptor/adaptors/orig-includes.h"

namespace darknet_adaptor::adaptors::layers
{

namespace
{

using buffers = LayersBuffers;

}  // namespace

ALayerAdaptorPtr LocalLayer::make(int batch, int h, int w, int c, int n,
                                  int size, int stride, int pad,
                                  ACTIVATION activation)
{
  ALayerAdaptorPtr adaptor = ALayerAdaptor::create();
  layer& l = *adaptor->get();

  l.type = LOCAL;

  l.h = h;
  l.w = w;
  l.c = c;
  l.n = n;
  l.batch = batch;
  l.stride = stride;
  l.size = size;
  l.pad = pad;

  const int out_h = local_out_height(l);
  const int out_w = local_out_width(l);
  const int locations = out_h * out_w;
  l.out_h = out_h;
  l.out_w = out_w;
  l.out_c = n;
  l.outputs = l.out_h * l.out_w * l.out_c;
  l.inputs = l.w * l.h * l.c;

  const size_t nweights =
      buffers::count(c, n) * buffers::count(size, size, locations);

  adaptor->weights(buffers::floats(nweights));
  adaptor->weight_updates(buffers::floats(nweights));

  adaptor->biases(buffers::floats(buffers::count(l.outputs)));
  adaptor->bias_updates(buffers::floats(buffers::count(l.outputs)));

  const float scale = static_cast<float>(std::sqrt(2. / (size * size * c)));
  for (int i = 0; i < c * n * size * size; ++i) {
    l.weights[i] = scale * rand_uniform(-1, 1);
  }

  // The im2col() of the computation takes the pad flag for the padding, so
  // it's columns of the pad=1 and the size < 3 outgrow the locations (the
  // original wrote them past the col_image end).
  const int col_h = (h + 2 * pad - size) / stride + 1;
  const int col_w = (w + 2 * pad - size) / stride + 1;
  const size_t columns =
      std::max(buffers::count(out_h, out_w),
               buffers::count(col_h > 0 ? col_h : 0, col_w > 0 ? col_w : 0));

  adaptor->col_image(buffers::floats(columns * buffers::count(size, size, c)));
  adaptor->output(buffers::floats(buffers::count(l.batch, out_h, out_w, n)));
  adaptor->delta(buffers::floats(buffers::count(l.batch, out_h, out_w, n)));

  l.forward = forward_local_layer;
  l.backward = backward_local_layer;
  l.update = update_local_layer;

  l.activation = activation;

  CLOGI("Local Layer: %d x %d x %d image, %d filters -> %d x %d x %d image", h,
        w, c, n, out_h, out_w, n);

  return adaptor;
}

}  // namespace darknet_adaptor::adaptors::layers
