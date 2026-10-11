#ifndef DARKNET_CXX_PORT_PROJECT_DXXWJZ2_V2_FORMAT_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_DXXWJZ2_V2_FORMAT_CLASS_H

#include <cstdint>

/**
 * @brief The dxxwjz2 (the darknetxx weights json zipped) network weights
 * files of the second version: the dxxwjz1 document (see the Dxxwjz1Format)
 * of the network, which keeps everything the network is detected and
 * trained with beside the weights:
 *
 * @code
 * {
 *   "format": "dxxwjz2",
 *   "version": 2,
 *   "generator": "darknetxx 0.11.0",
 *   "network": {"cfg": "[net]\nbatch=64\n...", "classes": [
 *     {"index": 0, "name": "person"},
 *     {"index": 1, "name": "bicycle"}
 *   ]},
 *   "training": {"seconds": 3725,
 *     "iteration": [1, 2, 3],
 *     "loss": [812.5, 790.25, 701],
 *     "avg_loss": [812.5, 810.27, 799.34],
 *     "map_iteration": [3],
 *     "map": [0.0125]
 *   },
 *   "header": {"major": 0, "minor": 2, "revision": 5, "seen": 192},
 *   "layers": [...],
 *   "trailing": "AAE="
 * }
 * @endcode
 *
 * - The network keeps the text of the cfg file of the network (required) and
 *   the names of the classes at their indexes, every index from zero once.
 * - The training keeps the seconds the training took (the resumed trainings
 *   add theirs) and the loss of every training iteration with its average,
 *   in the order of the training, and the mAP of every mAP calculation of
 *   the training (the optional map columns, of the ascending iterations).
 * - The header, the layers and the trailing bytes are the dxxwjz1 ones; the
 *   file without them keeps no weights, it's the network to train.
 */
namespace darknet_adaptor::loaders::dxxwjz2::v2
{

/// @brief The format name and version constants of the dxxwjz2 files.
class Dxxwjz2Format
{
 public:
  /// @brief The value of the format key.
  inline static constexpr const char* const NAME = "dxxwjz2";
  /// @brief The value of the version key.
  inline static constexpr const int64_t VERSION = 2;
  /// @brief The file name extension of the dxxwjz2 files.
  inline static constexpr const char* const EXTENSION = ".dxxwjz2";

  /// @brief The JSON keys beside the dxxwjz1 ones.
  inline static constexpr const char* const NETWORK_KEY = "network";
  inline static constexpr const char* const CFG_KEY = "cfg";
  inline static constexpr const char* const CLASSES_KEY = "classes";
  inline static constexpr const char* const NAME_KEY = "name";
  inline static constexpr const char* const TRAINING_KEY = "training";
  inline static constexpr const char* const SECONDS_KEY = "seconds";
  inline static constexpr const char* const ITERATION_KEY = "iteration";
  inline static constexpr const char* const LOSS_KEY = "loss";
  inline static constexpr const char* const AVG_LOSS_KEY = "avg_loss";
  inline static constexpr const char* const MAP_ITERATION_KEY = "map_iteration";
  inline static constexpr const char* const MAP_KEY = "map";
};

}  // namespace darknet_adaptor::loaders::dxxwjz2::v2

#endif  // DARKNET_CXX_PORT_PROJECT_DXXWJZ2_V2_FORMAT_CLASS_H
