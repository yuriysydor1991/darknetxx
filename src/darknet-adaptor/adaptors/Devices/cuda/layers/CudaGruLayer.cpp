#include "src/darknet-adaptor/adaptors/Devices/cuda/layers/CudaGruLayer.h"

#include "src/darknet-adaptor/adaptors/Devices/cuda/CudaArrays.h"
#include "src/darknet-adaptor/adaptors/Layers/LayersBuffers.h"

namespace darknet_adaptor::adaptors::devices::cuda
{

using layers::LayersBuffers;

void CudaGruLayer::place(ALayerAdaptor& adaptor, ANetworkAdaptor& net,
                         int index) const
{
  layer& l = *adaptor.get();
  const CudaArrays arrays{adaptor};
  const size_t state = LayersBuffers::count(l.batch, l.outputs);
  const size_t steps = state * LayersBuffers::count(l.steps);

  l.forward_gpu = forward_gru_layer_gpu;
  l.backward_gpu = backward_gru_layer_gpu;
  l.update_gpu = update_gru_layer_gpu;

  for (float* layer::* array :
       {&layer::forgot_state_gpu, &layer::forgot_delta_gpu,
        &layer::prev_state_gpu, &layer::state_gpu, &layer::r_gpu, &layer::z_gpu,
        &layer::h_gpu}) {
    arrays.make(l.*array, state);
  }

  arrays.make(l.output_gpu, l.output, steps);
  arrays.make(l.delta_gpu, l.delta, steps);

  l.workspace_size =
      place_all({adaptor.input_z_layer(), adaptor.input_r_layer(),
                 adaptor.input_h_layer(), adaptor.state_z_layer(),
                 adaptor.state_r_layer(), adaptor.state_h_layer()},
                net, index);
}

void CudaGruLayer::push(const layer& l) const
{
  push_all({l.input_z_layer, l.input_r_layer, l.input_h_layer, l.state_z_layer,
            l.state_r_layer, l.state_h_layer});
}

void CudaGruLayer::pull(const layer& l) const
{
  pull_all({l.input_z_layer, l.input_r_layer, l.input_h_layer, l.state_z_layer,
            l.state_r_layer, l.state_h_layer});
}

}  // namespace darknet_adaptor::adaptors::devices::cuda
