#ifndef DARKNET_CXX_PORT_PROJECT_DXXWJZ3_V3_LOSSES_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_DXXWJZ3_V3_LOSSES_CLASS_H

#include <vector>

namespace darknet_adaptor::loaders::dxxwjz3::v3
{

/**
 * @brief The loss columns of the training member of the dxxwjz3 files (see
 * the Dxxwjz3Format), which keep the loss of the iteration at the index of
 * the iteration - 1 instead of the iteration column.
 */
class Dxxwjz3Losses
{
 public:
  /// @brief The values of the iterations at their indexes, the NaN ones of
  /// the iterations of none; the iterations before the first one are left
  /// out.
  static std::vector<float> indexed(const std::vector<int>& iterations,
                                    const std::vector<float>& values);
};

}  // namespace darknet_adaptor::loaders::dxxwjz3::v3

#endif  // DARKNET_CXX_PORT_PROJECT_DXXWJZ3_V3_LOSSES_CLASS_H
