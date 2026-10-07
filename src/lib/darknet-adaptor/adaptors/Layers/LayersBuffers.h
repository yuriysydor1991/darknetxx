#ifndef DARKNET_CXX_PORT_PROJECT_LAYERS_BUFFERS_HELPERS_H
#define DARKNET_CXX_PORT_PROJECT_LAYERS_BUFFERS_HELPERS_H

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace darknet_adaptor::adaptors::layers
{

/**
 * @brief The buffers of the layer adaptors: the C++ counterparts of the
 * xcalloc() and xrealloc() calls of the original darknet layers.
 */
class LayersBuffers
{
 public:
  /**
   * @brief The element count of the buffer darknet allocates for the product
   * of the given int factors (computed without the int overflow).
   *
   * Throws NetworkError for a negative factor, where xcalloc() failed.
   */
  static size_t count(int64_t first, int64_t second = 1, int64_t third = 1,
                      int64_t fourth = 1);

  /// @brief The zeroed floats buffer, the xcalloc(count, sizeof(float)).
  static std::vector<float> floats(size_t count, float value = 0.f);

  /// @brief The zeroed int buffer, the xcalloc(count, sizeof(int)).
  static std::vector<int> ints(size_t count, int value = 0);

  /// @brief The zeroed bytes buffer, the xcalloc(count, sizeof(char)).
  static std::string bytes(size_t count);

  /**
   * @brief The xrealloc() of the buffer to the new count: the leading values
   * are kept, the new tail is zeroed (instead of the indeterminate one).
   */
  template <typename T>
  static std::vector<T> resized(const std::vector<T>& old, size_t count)
  {
    std::vector<T> buffer(count);
    std::copy_n(old.begin(), std::min(count, old.size()), buffer.begin());
    return buffer;
  }
};

}  // namespace darknet_adaptor::adaptors::layers

#endif  // DARKNET_CXX_PORT_PROJECT_LAYERS_BUFFERS_HELPERS_H
