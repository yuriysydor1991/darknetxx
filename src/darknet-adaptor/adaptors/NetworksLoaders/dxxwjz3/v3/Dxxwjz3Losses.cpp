#include "src/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz3/v3/Dxxwjz3Losses.h"

#include <cstddef>
#include <limits>
#include <vector>

namespace darknet_adaptor::loaders::dxxwjz3::v3
{

std::vector<float> Dxxwjz3Losses::indexed(const std::vector<int>& iterations,
                                          const std::vector<float>& values)
{
  std::vector<float> column;

  for (std::size_t index = 0U; index < iterations.size(); ++index) {
    if (iterations[index] < 1) {
      continue;
    }

    const auto at = static_cast<std::size_t>(iterations[index]) - 1U;

    if (at >= column.size()) {
      column.resize(at + 1U, std::numeric_limits<float>::quiet_NaN());
    }

    column[at] = values[index];
  }

  return column;
}

}  // namespace darknet_adaptor::loaders::dxxwjz3::v3
