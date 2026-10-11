#include "src/darknet-adaptor/adaptors/Layers/LayersBuffers.h"

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include "src/darknet-adaptor/adaptors/NetworkError.h"

namespace darknet_adaptor::adaptors::layers
{

size_t LayersBuffers::count(int64_t first, int64_t second, int64_t third,
                            int64_t fourth)
{
  if (first < 0 || second < 0 || third < 0 || fourth < 0) {
    throw NetworkError(
        "Invalid negative layer buffer size, the layer parameters don't "
        "match it's input");
  }

  return static_cast<size_t>(first) * static_cast<size_t>(second) *
         static_cast<size_t>(third) * static_cast<size_t>(fourth);
}

std::vector<float> LayersBuffers::floats(size_t count, float value)
{
  return std::vector<float>(count, value);
}

std::vector<int> LayersBuffers::ints(size_t count, int value)
{
  return std::vector<int>(count, value);
}

std::string LayersBuffers::bytes(size_t count)
{
  return std::string(count, '\0');
}

}  // namespace darknet_adaptor::adaptors::layers
