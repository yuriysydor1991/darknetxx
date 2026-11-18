#ifndef DARKNET_CXX_PORT_PROJECT_CUDA_ARRAYS_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_CUDA_ARRAYS_CLASS_H

#include <cstddef>

#include "src/darknet-adaptor/adaptors/ResourcesKeeper.h"

namespace darknet_adaptor::adaptors::devices::cuda
{

/**
 * @brief The device arrays of the struct fields the keeper (the layer or the
 * network adaptor) keeps: a new array replaces the previous one of the field.
 */
class CudaArrays
{
 public:
  explicit CudaArrays(ResourcesKeeper& keeper);

  /// @brief The array of the n elements copied from the host, zeroed for none.
  template <typename T>
  void make(T*& array, const T* host, size_t n) const
  {
    release(array);
    array = static_cast<T*>(allocate(host, n * sizeof(T)));
    keep(array);
  }

  template <typename T>
  void make(T*& array, size_t n) const
  {
    make(array, static_cast<const T*>(nullptr), n);
  }

  /// @brief The make() of the field of no array yet.
  template <typename T>
  void once(T*& array, const T* host, size_t n) const
  {
    if (array == nullptr) make(array, host, n);
  }

  template <typename T>
  void once(T*& array, size_t n) const
  {
    if (array == nullptr) make(array, n);
  }

  /**
   * @brief The zeroed page locked host array of the n floats, the regular
   * one where it fails. Tells if it is the page locked one.
   */
  bool pinned(float*& array, size_t n) const;

  void release(const void* array) const;

 private:
  static void* allocate(const void* host, size_t bytes);
  void keep(void* array) const;

  ResourcesKeeper& mkeeper;
};

}  // namespace darknet_adaptor::adaptors::devices::cuda

#endif  // DARKNET_CXX_PORT_PROJECT_CUDA_ARRAYS_CLASS_H
