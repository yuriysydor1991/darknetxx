#ifndef DARKNETXX_PROJECT_CTEST_ORIG_WEIGHTS_FILE_DARKNET_NETWORK_H
#define DARKNETXX_PROJECT_CTEST_ORIG_WEIGHTS_FILE_DARKNET_NETWORK_H

#include <gtest/gtest.h>

#include <cstdint>
#include <string>

#include "WeightsFile.h"
#include "WeightsNetSpec.h"
#include "src/darknet-adaptor/adaptors/ANetworkAdaptor/ANetworkAdaptor.h"
#include "src/darknet.h"
#include "src/network.h"

namespace weights_test
{

// the generated networks and weights files of the weights factory
using namespace weights_factory;

/**
 * @brief Owns the network the OrigDefaultLoader builds of the cfg file and
 * accesses it's float arrays by the weights file layout blocks.
 */
class DarknetNetwork
{
 public:
  DarknetNetwork(const std::string& cfg, ParseMode mode);

  /// @brief Owns the network the OrigDefaultLoader::load_network() and the
  /// OrigDefaultLoader::load_network_custom() calls return.
  explicit DarknetNetwork(darknet_adaptor::adaptors::ANetworkAdaptorPtr loaded);

  DarknetNetwork(const DarknetNetwork&) = delete;
  DarknetNetwork& operator=(const DarknetNetwork&) = delete;

  network& net();

  /// @brief The layer or the sub-layer of the block, nullptr for none.
  layer* layer_of(const Block& block);

  /// @brief The float array of the block, nullptr when the layer has none.
  float* array(const Block& block);

  /// @brief Stores the pattern values into the arrays of the layout.
  ::testing::AssertionResult fill(const Layout& layout, const Pattern& pattern);

  /// @brief Checks the arrays of the layout hold the pattern values.
  ::testing::AssertionResult holds(const Layout& layout,
                                   const Pattern& pattern);

  /**
   * @brief Checks the arrays after the load of a weights file.
   *
   * @param memory The arrays of the network, which held the before values.
   * @param before The values of the arrays before the load.
   * @param loaded The arrays the loader reads, in the reading order.
   * @param file The payload of the loaded file.
   * @param payload_bytes The payload bytes the file holds, less than the file
   * model ones for the truncated file.
   *
   * The loaded arrays get the file floats at their payload offsets, the
   * loader transposition applied. The floats beyond the end of the file and
   * the arrays the loader skips keep the before values, while the float the
   * file holds a part of is indeterminate. The loader stops after the layer,
   * which reads past the end of the file (it's arrays get transposed even
   * without the data), the layers after it stay as they are.
   */
  ::testing::AssertionResult holds_loaded(const Layout& memory,
                                          const Pattern& before,
                                          const Layout& loaded,
                                          const FileModel& file,
                                          uint64_t payload_bytes);

 private:
  darknet_adaptor::adaptors::ANetworkAdaptorPtr mnet;
};

}  // namespace weights_test

#endif  // DARKNETXX_PROJECT_CTEST_ORIG_WEIGHTS_FILE_DARKNET_NETWORK_H
