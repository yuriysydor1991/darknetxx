#include "src/darknet-adaptor/adaptors/Devices/cuda/layers/CudaConvLstmLayer.h"

#include <algorithm>

#include "src/darknet-adaptor/adaptors/Devices/cuda/CudaArrays.h"
#include "src/darknet-adaptor/adaptors/Layers/LayersBuffers.h"

namespace darknet_adaptor::adaptors::devices::cuda
{

using layers::LayersBuffers;

void CudaConvLstmLayer::place(ALayerAdaptor& adaptor, ANetworkAdaptor& net,
                              int index) const
{
  layer& l = *adaptor.get();
  const CudaArrays arrays{adaptor};
  const size_t state = LayersBuffers::count(l.batch, l.outputs);
  const size_t steps = state * LayersBuffers::count(l.steps);

  l.forward_gpu = forward_conv_lstm_layer_gpu;
  l.backward_gpu = backward_conv_lstm_layer_gpu;
  l.update_gpu = update_conv_lstm_layer_gpu;

  for (float* layer::* array :
       {&layer::output_gpu, &layer::delta_gpu, &layer::cell_gpu}) {
    arrays.make(l.*array, steps);
  }

  for (float* layer::* array :
       {&layer::prev_state_gpu, &layer::prev_cell_gpu, &layer::f_gpu,
        &layer::i_gpu, &layer::g_gpu, &layer::o_gpu, &layer::c_gpu,
        &layer::h_gpu, &layer::stored_c_gpu, &layer::stored_h_gpu,
        &layer::temp_gpu, &layer::temp2_gpu, &layer::temp3_gpu, &layer::dc_gpu,
        &layer::dh_gpu, &layer::last_prev_state_gpu,
        &layer::last_prev_cell_gpu}) {
    arrays.make(l.*array, state);
  }

  size_t workspace = place_all(
      {adaptor.wf(), adaptor.uf(), adaptor.ui(), adaptor.ug(), adaptor.uo()},
      net, index);

  if (l.peephole) {
    workspace = std::max(
        workspace,
        place_all({adaptor.vf(), adaptor.vi(), adaptor.vo()}, net, index));
  }

  if (l.bottleneck) {
    arrays.make(l.bottelneck_hi_gpu, state * 2U);
    arrays.make(l.bottelneck_delta_gpu, state * 2U);
  } else {
    workspace = std::max(
        workspace,
        place_all({adaptor.wi(), adaptor.wg(), adaptor.wo()}, net, index));
  }

  l.workspace_size = workspace;
}

void CudaConvLstmLayer::push(const layer& l) const
{
  push_all({l.wf, l.uf, l.ui, l.ug, l.uo});
  if (l.peephole) push_all({l.vf, l.vi, l.vo});
  if (!l.bottleneck) push_all({l.wi, l.wg, l.wo});
}

void CudaConvLstmLayer::pull(const layer& l) const
{
  pull_all({l.wf, l.uf, l.ui, l.ug, l.uo});
  if (l.peephole) pull_all({l.vf, l.vi, l.vo});
  if (!l.bottleneck) pull_all({l.wi, l.wg, l.wo});
}

}  // namespace darknet_adaptor::adaptors::devices::cuda
