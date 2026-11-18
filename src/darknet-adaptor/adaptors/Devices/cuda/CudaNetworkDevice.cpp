#include "src/darknet-adaptor/adaptors/Devices/cuda/CudaNetworkDevice.h"

#include <memory>

#include "src/darknet-adaptor/adaptors/ANetworkAdaptor/ANetworkAdaptor.h"
#include "src/darknet-adaptor/adaptors/Devices/cuda/CudaArrays.h"
#include "src/darknet-adaptor/adaptors/Devices/cuda/CudaNetworkInputs.h"
#include "src/darknet-adaptor/adaptors/Devices/cuda/layers/CudaLayers.h"
#include "src/darknet-adaptor/adaptors/orig-includes.h"
#include "src/log/log.h"

namespace darknet_adaptor::adaptors::devices::cuda
{

CudaNetworkDevice::CudaNetworkDevice()
{
  cuda_set_device(gpu_index);
  CHECK_CUDA(cudaSetDeviceFlags(cudaDeviceScheduleBlockingSync));
  show_cuda_cudnn_info();
}

bool CudaNetworkDevice::available()
{
  static const bool present = [] {
    int count = 0;

    if (gpu_index >= 0 && cudaGetDeviceCount(&count) == cudaSuccess &&
        count > 0) {
      return true;
    }

    cudaGetLastError();
    LOGW("No CUDA GPU found, the networks compute on the CPU");
    gpu_index = -1;
    return false;
  }();

  return present;
}

void CudaNetworkDevice::select(int gpu) const { cuda_set_device(gpu); }

void CudaNetworkDevice::place(ANetworkAdaptor& net) const
{
  network& view = *net.get();

  cuda_set_device(view.gpu_index);

  if (view.input_gpu == nullptr) {
    describe(view);
    net.keep(std::make_shared<CudaNetworkInputs>(view));
  } else {
    CudaNetworkInputs::reset(view);
  }

  place_layers(net);
  place_inputs(net);
  place_workspace(net);
}

void CudaNetworkDevice::push(const network& net) const
{
  cuda_set_device(net.gpu_index);

  for (int i = 0; i < net.n; ++i) {
    CudaLayers::of(net.layers[i].type).push(net.layers[i]);
  }
}

void CudaNetworkDevice::pull(const network& net) const
{
  cuda_set_device(net.gpu_index);

  for (int i = 0; i < net.n; ++i) {
    CudaLayers::of(net.layers[i].type).pull(net.layers[i]);
  }
}

void CudaNetworkDevice::describe(network& net)
{
  char name[256]{};
  const int capability = get_gpu_compute_capability(net.gpu_index, name);

#ifdef CUDNN_HALF
  net.cudnn_half = capability >= 700;
#endif

  LOGI(net.gpu_index << " : compute_capability = " << capability
                     << ", cudnn_half = " << net.cudnn_half
                     << ", GPU: " << name);

  // the GPU computation of the delta arrays of every layer
  if (net.optimized_memory) {
    LOGW("The [net] optimized_memory is not supported, ignoring it");
    net.optimized_memory = 0;
  }
}

void CudaNetworkDevice::place_layers(ANetworkAdaptor& net)
{
  network& view = *net.get();

  for (int i = 0; i < view.n; ++i) {
    const ICudaLayer& kind = CudaLayers::of(view.layers[i].type);
    const ALayerAdaptorPtr adaptor = net.layer_adaptor(i);

    if (adaptor) {
      kind.place(*adaptor, net, i);
    } else {
      // the layer of another network (the mAP calculation one), computing
      // the batch of it's own
      kind.describe(view.layers[i], net);
    }
  }
}

void CudaNetworkDevice::place_inputs(ANetworkAdaptor& net)
{
  network& view = *net.get();
  const CudaArrays arrays{net};
  const size_t size = static_cast<size_t>(get_network_input_size(view)) *
                      static_cast<size_t>(view.batch);

  arrays.make(view.input_state_gpu, size);
  view.input_pinned_cpu_flag = arrays.pinned(view.input_pinned_cpu, size);
}

void CudaNetworkDevice::place_workspace(ANetworkAdaptor& net)
{
  network& view = *net.get();
  const CudaArrays arrays{net};
  const size_t bytes = net.layers_workspace();

  // the device workspace of the previous placement and the host one of the
  // network made on the host
  arrays.release(view.workspace);
  net.workspace_bytes(0U);

  if (bytes > 0U) {
    arrays.make(view.workspace, bytes / sizeof(float) + 1U);
  }
}

}  // namespace darknet_adaptor::adaptors::devices::cuda
