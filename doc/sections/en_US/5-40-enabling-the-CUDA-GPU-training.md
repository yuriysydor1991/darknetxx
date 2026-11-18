## Enabling the CUDA GPU training

The networks train and detect on the NVIDIA GPUs once the project is built with the CUDA toolkit (and the CMake 3.23 or newer):

```
cmake -S . -B build -DENABLE_CUDA=ON
```

The [cmake/enablers/darknetxx-CUDA-enabler.cmake](/cmake/enablers/darknetxx-CUDA-enabler.cmake) module compiles the CUDA kernels of the original Darknet (the [`src/detector-cmake-adaptor/darknet/src/cuda`](/src/detector-cmake-adaptor/darknet/src/cuda) directory) with it's CUDA runtime (the `dark_cuda.c`), defines the `GPU` (and the `CUDNN`, the `CUDNN_HALF`) of every target and links the cuBLAS, the cuRAND and the cuDNN libraries to them. The cuDNN is taken once found, the `CUDNN_ROOT_DIR` environment variable points the configure at the one outside of the CUDA toolkit:

```
CUDNN_ROOT_DIR=/opt/cudnn cmake -S . -B build -DENABLE_CUDA=ON
```

The kernels are compiled for every major GPU architecture of the toolkit by default, the architecture of the GPU of the machine shortens the build:

```
cmake -S . -B build -DENABLE_CUDA=ON -DCMAKE_CUDA_ARCHITECTURES=native
```

| CMake option | Default | Effect |
| --- | --- | --- |
| `ENABLE_CUDA` | `OFF` | The training and the detection on the NVIDIA GPUs. |
| `ENABLE_CUDNN` | `ON` once the cuDNN is found | The convolutional, the connected, the batch normalization and the pooling layers computed by the cuDNN. |
| `ENABLE_CUDNN_HALF` | the `ENABLE_CUDNN` one | The half precision convolutions on the Tensor Cores of the GPUs of the compute capability 7.0 and up. |
| `CMAKE_CUDA_ARCHITECTURES` | `all-major` | The GPU architectures the kernels are compiled for. |

The binaries of the CUDA build take the CUDA runtime, the cuBLAS, the cuRAND (and the cuDNN) shared libraries of the toolkit at the run time.

### Training on the GPUs

The binaries of the CUDA build compute on the first GPU of the machine. The `--gpus` list of the GPU indexes trains a copy of the network on every GPU of it, the way the `-gpus` of the original Darknet did, while the detection computes on the first GPU of the list:

```
darknetxx --train-cfg project.dxxwjz4p --gpus 0,1
```

The networks the loader makes are placed on their GPU by the device of the [`src/darknet-adaptor/adaptors/Devices`](/src/darknet-adaptor/adaptors/Devices) component: every layer gets it's device arrays the way the original `make_*_layer()` allocated them, the resize of the random input sizes and the change of the batch place them again, the loaded weights are copied onto the GPU and the saved ones back from it. The machine of no CUDA GPU (or of no NVIDIA driver) computes on the CPU, warning about it.

The `optimized_memory` option of the `[net]` section isn't supported: the layers keep their whole device memory.
