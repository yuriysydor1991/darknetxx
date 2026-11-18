#include "src/darknet-adaptor/adaptors/Devices/cuda/CudaArrays.h"

#include <cstdlib>
#include <cstring>

#include "src/darknet-adaptor/adaptors/orig-includes.h"
#include "src/log/log.h"

namespace darknet_adaptor::adaptors::devices::cuda
{

CudaArrays::CudaArrays(ResourcesKeeper& keeper) : mkeeper{keeper} {}

bool CudaArrays::pinned(float*& array, size_t n) const
{
  release(array);

  void* host = nullptr;
  const size_t bytes = n * sizeof(float);

  if (cudaHostAlloc(&host, bytes, cudaHostAllocMapped) == cudaSuccess) {
    std::memset(host, 0, bytes);
    array = static_cast<float*>(host);
    mkeeper.keep(ResourcesKeeper::Resource(host, cudaFreeHost));
    return true;
  }

  // the regular memory instead, the error is none of the computation
  cudaGetLastError();
  array = static_cast<float*>(xcalloc(n, sizeof(float)));
  mkeeper.keep(ResourcesKeeper::Resource(array, std::free));
  return false;
}

void CudaArrays::release(const void* array) const { mkeeper.release(array); }

void* CudaArrays::allocate(const void* host, size_t bytes)
{
  void* array = nullptr;

  if (bytes == 0U) {
    return array;
  }

  const cudaError_t status = cudaMalloc(&array, bytes);

  if (status != cudaSuccess) {
    LOGE("Out of the GPU memory, try to set the subdivisions=64 of the cfg");
  }

  CHECK_CUDA(status);

  if (host != nullptr) {
    CHECK_CUDA(cudaMemcpyAsync(array, host, bytes, cudaMemcpyHostToDevice,
                               get_cuda_stream()));
  } else {
    CHECK_CUDA(cudaMemsetAsync(array, 0, bytes, get_cuda_stream()));
  }

  return array;
}

void CudaArrays::keep(void* array) const
{
  if (array != nullptr) {
    mkeeper.keep(ResourcesKeeper::Resource(array, cudaFree));
  }
}

}  // namespace darknet_adaptor::adaptors::devices::cuda
