cmake_minimum_required(VERSION 3.13)

# ---------------------------------------------------------------------------
# The NVIDIA GPU training and detection: the CUDA kernels of the original
# Darknet with the cuBLAS, the cuRAND and optionally the cuDNN libraries. The
# GPU (and the CUDNN) definition switches every darknet header and source to
# their GPU code, so all the targets of the project compile and link with them.
#
# The ENABLE_CUDNN option is ON by default once the cuDNN is found.
# ---------------------------------------------------------------------------
option(
  ENABLE_CUDA
  "Set to ON to train and detect on the NVIDIA GPUs (requires the CUDA toolkit)"
  OFF
)

if(NOT ENABLE_CUDA)
  return()
endif()

if(CMAKE_VERSION VERSION_LESS 3.23)
  message(FATAL_ERROR "The ENABLE_CUDA build requires the CMake 3.23 or newer")
endif()

if(NOT DEFINED CMAKE_CUDA_ARCHITECTURES)
  set(CMAKE_CUDA_ARCHITECTURES all-major)
endif()

# the one CUDA runtime of the process, the one the C and C++ code links
set(CMAKE_CUDA_RUNTIME_LIBRARY Shared)

enable_language(CUDA)
find_package(CUDAToolkit REQUIRED)

add_compile_definitions(GPU)
link_libraries(CUDA::cudart CUDA::cublas CUDA::curand)

find_path(
  CUDNN_INCLUDE_DIR cudnn.h
  HINTS ${CUDAToolkit_INCLUDE_DIRS} ENV CUDNN_ROOT_DIR
  PATH_SUFFIXES include
)
find_library(
  CUDNN_LIBRARY cudnn
  HINTS ${CUDAToolkit_LIBRARY_DIR} ENV CUDNN_ROOT_DIR
  PATH_SUFFIXES lib lib64
)

if(CUDNN_INCLUDE_DIR AND CUDNN_LIBRARY)
  set(CUDNN_FOUND ON)
endif()

option(
  ENABLE_CUDNN
  "Set to ON to compute the layers by the cuDNN library (ON by default once found)"
  ${CUDNN_FOUND}
)

option(
  ENABLE_CUDNN_HALF
  "Set to ON to compute the convolutions in the half precision on the Tensor Cores of the GPUs of the compute capability 7.0 and up"
  ${ENABLE_CUDNN}
)

if(ENABLE_CUDNN)
  if(NOT CUDNN_FOUND)
    message(FATAL_ERROR
      "The cuDNN library is not found: point the CUDNN_ROOT_DIR environment "
      "variable at it or turn the ENABLE_CUDNN option OFF")
  endif()

  include_directories(${CUDNN_INCLUDE_DIR})
  link_libraries(${CUDNN_LIBRARY})
  add_compile_definitions(CUDNN)

  if(ENABLE_CUDNN_HALF)
    add_compile_definitions(CUDNN_HALF)
  endif()
endif()

message(STATUS "CUDA toolkit: ${CUDAToolkit_VERSION}")
message(STATUS "CUDA architectures: ${CMAKE_CUDA_ARCHITECTURES}")
message(STATUS "cuDNN: ${ENABLE_CUDNN} ${CUDNN_LIBRARY}, half: ${ENABLE_CUDNN_HALF}")
