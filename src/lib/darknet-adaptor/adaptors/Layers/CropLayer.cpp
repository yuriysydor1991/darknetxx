#include "src/lib/darknet-adaptor/adaptors/Layers/CropLayer.h"

#include "src/lib/darknet-adaptor/adaptors/ALayerAdaptor/ALayerAdaptor.h"
#include "src/lib/darknet-adaptor/adaptors/Layers/LayersBuffers.h"
#include "src/lib/darknet-adaptor/adaptors/orig-includes.h"

namespace darknet_adaptor::adaptors::layers
{

ALayerAdaptorPtr CropLayer::make(int batch, int h, int w, int c,
                                 int crop_height, int crop_width, int flip,
                                 float angle, float saturation, float exposure)
{
  CLOGI("Crop Layer: %d x %d -> %d x %d x %d image", h, w, crop_height,
        crop_width, c);

  ALayerAdaptorPtr adaptor = ALayerAdaptor::create();
  layer& l = *adaptor->get();

  l.type = CROP;
  l.batch = batch;
  l.h = h;
  l.w = w;
  l.c = c;
  l.scale = static_cast<float>(crop_height) / static_cast<float>(h);
  l.flip = flip;
  l.angle = angle;
  l.saturation = saturation;
  l.exposure = exposure;
  l.out_w = crop_width;
  l.out_h = crop_height;
  l.out_c = c;
  l.inputs = l.w * l.h * l.c;
  l.outputs = l.out_w * l.out_h * l.out_c;
  adaptor->output(
      LayersBuffers::floats(LayersBuffers::count(l.outputs, batch)));
  l.forward = forward_crop_layer;
  l.backward = backward_crop_layer;

  return adaptor;
}

void CropLayer::resize(ALayerAdaptor& adaptor, int w, int h)
{
  layer& l = *adaptor.get();

  l.w = w;
  l.h = h;

  l.out_w = static_cast<int>(l.scale * static_cast<float>(w));
  l.out_h = static_cast<int>(l.scale * static_cast<float>(h));

  l.inputs = l.w * l.h * l.c;
  l.outputs = l.out_h * l.out_w * l.out_c;

  adaptor.output(LayersBuffers::resized(
      adaptor.output(), LayersBuffers::count(l.batch, l.outputs)));
}

}  // namespace darknet_adaptor::adaptors::layers
