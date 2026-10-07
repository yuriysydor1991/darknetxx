#include "src/lib/darknet-adaptor/adaptors/Layers/RouteLayer.h"

#include <utility>
#include <vector>

#include "src/lib/darknet-adaptor/adaptors/ALayerAdaptor/ALayerAdaptor.h"
#include "src/lib/darknet-adaptor/adaptors/Layers/LayersBuffers.h"
#include "src/lib/darknet-adaptor/adaptors/NetworkError.h"
#include "src/lib/darknet-adaptor/adaptors/orig-includes.h"

namespace darknet_adaptor::adaptors::layers
{

namespace
{

using buffers = LayersBuffers;

}  // namespace

ALayerAdaptorPtr RouteLayer::make(int batch, std::vector<int> input_layers,
                                  std::vector<int> input_sizes, int groups,
                                  int group_id)
{
  CLOGIM("route ");

  ALayerAdaptorPtr adaptor = ALayerAdaptor::create();
  layer& l = *adaptor->get();

  const int n = static_cast<int>(input_layers.size());

  l.type = ROUTE;
  l.batch = batch;
  l.n = n;
  adaptor->input_layers(std::move(input_layers));
  adaptor->input_sizes(std::move(input_sizes));
  l.groups = groups;
  l.group_id = group_id;
  l.wait_stream_id = -1;

  int outputs = 0;
  for (int i = 0; i < n; ++i) {
    CLOGI(" input_layers[%d] = %d", i, l.input_layers[i]);
    outputs += l.input_sizes[i];
  }

  if (groups == 0) {
    throw NetworkError("The groups of the [route] layer can't be 0");
  }

  outputs = outputs / groups;
  l.outputs = outputs;
  l.inputs = outputs;

  CLOGT(" inputs = %d \t outputs = %d, groups = %d, group_id = %d", l.inputs,
        l.outputs, l.groups, l.group_id);

  adaptor->delta(buffers::floats(buffers::count(outputs, batch)));
  adaptor->output(buffers::floats(buffers::count(outputs, batch)));

  l.forward = forward_route_layer;
  l.backward = backward_route_layer;

  return adaptor;
}

void RouteLayer::resize(ALayerAdaptor& adaptor, const network& net)
{
  layer& l = *adaptor.get();

  const layer& first = net.layers[l.input_layers[0]];
  // the input size of the first input layer, as the parser set it (the
  // original kept the initial one)
  l.w = first.w;
  l.h = first.h;
  l.out_w = first.out_w;
  l.out_h = first.out_h;
  l.out_c = first.out_c;
  l.outputs = first.outputs;
  l.input_sizes[0] = first.outputs;

  for (int i = 1; i < l.n; ++i) {
    const int index = l.input_layers[i];
    const layer& next = net.layers[index];
    l.outputs += next.outputs;
    l.input_sizes[i] = next.outputs;
    if (next.out_w == first.out_w && next.out_h == first.out_h) {
      l.out_c += next.out_c;
    } else {
      CLOGE("Error: Different size of input layers: %d x %d, %d x %d",
            next.out_w, next.out_h, first.out_w, first.out_h);
      throw NetworkError("Different size of the [route] input layers");
    }
  }

  l.out_c = l.out_c / l.groups;
  l.outputs = l.outputs / l.groups;
  l.inputs = l.outputs;

  const size_t size = buffers::count(l.outputs, l.batch);

  adaptor.delta(buffers::resized(adaptor.delta(), size));
  adaptor.output(buffers::resized(adaptor.output(), size));
}

}  // namespace darknet_adaptor::adaptors::layers
