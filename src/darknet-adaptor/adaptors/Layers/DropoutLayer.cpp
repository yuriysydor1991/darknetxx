#include "src/darknet-adaptor/adaptors/Layers/DropoutLayer.h"

#include "src/darknet-adaptor/adaptors/ALayerAdaptor/ALayerAdaptor.h"
#include "src/darknet-adaptor/adaptors/Layers/LayersBuffers.h"
#include "src/darknet-adaptor/adaptors/NetworkError.h"
#include "src/darknet-adaptor/adaptors/orig-includes.h"

namespace darknet_adaptor::adaptors::layers
{

ALayerAdaptorPtr DropoutLayer::make(int batch, int inputs, float probability,
                                    int dropblock, float dropblock_size_rel,
                                    int dropblock_size_abs, int w, int h, int c)
{
  ALayerAdaptorPtr adaptor = ALayerAdaptor::create();
  layer& l = *adaptor->get();

  l.type = DROPOUT;
  l.probability = probability;
  l.dropblock = dropblock;
  l.dropblock_size_rel = dropblock_size_rel;
  l.dropblock_size_abs = dropblock_size_abs;

  if (l.dropblock) {
    l.out_w = l.w = w;
    l.out_h = l.h = h;
    l.out_c = l.c = c;

    if (l.w <= 0 || l.h <= 0 || l.c <= 0) {
      CLOGE(
          " Error: DropBlock - there must be positive values for: l.w=%d, "
          "l.h=%d, l.c=%d",
          l.w, l.h, l.c);
      throw NetworkError("DropBlock needs the positive input dimensions");
    }
  }

  l.inputs = inputs;
  l.outputs = inputs;
  l.batch = batch;
  adaptor->rand(LayersBuffers::floats(LayersBuffers::count(inputs, batch)));
  l.scale = static_cast<float>(1. / (1.0 - static_cast<double>(probability)));
  l.forward = forward_dropout_layer;
  l.backward = backward_dropout_layer;

  if (l.dropblock) {
    if (l.dropblock_size_abs) {
      CLOGI(
          "dropblock    p = %.3f   l.dropblock_size_abs = %d    %4d  ->   "
          "%4d\n",
          static_cast<double>(probability), l.dropblock_size_abs, inputs,
          inputs);
    } else {
      CLOGI(
          "dropblock    p = %.3f   l.dropblock_size_rel = %.2f    %4d  ->  "
          " %4d\n",
          static_cast<double>(probability),
          static_cast<double>(l.dropblock_size_rel), inputs, inputs);
    }
  } else {
    CLOGI("dropout    p = %.3f        %4d  ->   %4d\n",
          static_cast<double>(probability), inputs, inputs);
  }

  return adaptor;
}

void DropoutLayer::resize(ALayerAdaptor& adaptor, int inputs)
{
  layer& l = *adaptor.get();

  l.inputs = l.outputs = inputs;
  adaptor.rand(LayersBuffers::resized(adaptor.rand(),
                                      LayersBuffers::count(l.inputs, l.batch)));
}

}  // namespace darknet_adaptor::adaptors::layers
