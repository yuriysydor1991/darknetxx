#include "src/darknet-adaptor/adaptors/Devices/cuda/layers/CudaLstmLayer.h"

#include "src/darknet-adaptor/adaptors/Devices/cuda/CudaArrays.h"
#include "src/darknet-adaptor/adaptors/Layers/LayersBuffers.h"

namespace darknet_adaptor::adaptors::devices::cuda
{

using layers::LayersBuffers;

void CudaLstmLayer::place(ALayerAdaptor& adaptor, ANetworkAdaptor& net,
                          int index) const
{
  layer& l = *adaptor.get();
  const CudaArrays arrays{adaptor};
  const size_t state = LayersBuffers::count(l.batch, l.outputs);
  const size_t steps = state * LayersBuffers::count(l.steps);

  l.forward_gpu = forward_lstm_layer_gpu;
  l.backward_gpu = backward_lstm_layer_gpu;
  l.update_gpu = update_lstm_layer_gpu;

  for (float* layer::* array :
       {&layer::output_gpu, &layer::delta_gpu, &layer::cell_gpu}) {
    arrays.make(l.*array, steps);
  }

  for (float* layer::* array :
       {&layer::prev_state_gpu, &layer::prev_cell_gpu, &layer::f_gpu,
        &layer::i_gpu, &layer::g_gpu, &layer::o_gpu, &layer::c_gpu,
        &layer::h_gpu, &layer::temp_gpu, &layer::temp2_gpu, &layer::temp3_gpu,
        &layer::dc_gpu, &layer::dh_gpu}) {
    arrays.make(l.*array, state);
  }

  l.workspace_size =
      place_all({adaptor.wf(), adaptor.wi(), adaptor.wg(), adaptor.wo(),
                 adaptor.uf(), adaptor.ui(), adaptor.ug(), adaptor.uo()},
                net, index);
}

void CudaLstmLayer::push(const layer& l) const
{
  push_all({l.wf, l.wi, l.wg, l.wo, l.uf, l.ui, l.ug, l.uo});
}

void CudaLstmLayer::pull(const layer& l) const
{
  pull_all({l.wf, l.wi, l.wg, l.wo, l.uf, l.ui, l.ug, l.uo});
}

}  // namespace darknet_adaptor::adaptors::devices::cuda
