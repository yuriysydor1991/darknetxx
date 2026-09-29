#ifndef DARKNETXX_PROJECT_WEIGHTS_FACTORY_VARIANTS_H
#define DARKNETXX_PROJECT_WEIGHTS_FACTORY_VARIANTS_H

#include <cstddef>
#include <cstdint>
#include <ostream>
#include <string>
#include <vector>

#include "WeightsFile.h"
#include "WeightsNetSpec.h"

namespace weights_factory
{

/**
 * @brief One generated weights file variant: the network, the way darknet
 * parses it and the parameters of the files the tests make of it.
 */
struct WeightsVariant
{
  /// @brief The unique GoogleTest parameter name.
  std::string name;
  NetSpec spec;
  ParseMode mode{ParseMode::Train};
  /// @brief The header of the files written for the loader, the seen value
  /// included.
  WeightsHeader header;
  /// @brief The iteration the saved network is at.
  int iteration{1};
  /// @brief The pattern seed of the array values.
  uint64_t seed{1U};

  bool train() const;
};

/// @brief Keeps GoogleTest from dumping the variant bytes.
void PrintTo(const WeightsVariant& variant, std::ostream* os);

/// @brief The width x height network input.
struct Resolution
{
  int width{0};
  int height{0};
};

namespace variants
{

/// @brief The default seed and count of the random networks.
constexpr const uint64_t DEFAULT_RANDOM_SEED = 20260929U;
constexpr const size_t DEFAULT_RANDOM_COUNT = 160U;

/// @brief The network resolutions from the 2x2 pixels up to the DCI 4K.
std::vector<Resolution> resolutions();

/// @brief The weights file header versions: the 32 and the 64 bits seen ones,
/// the transposed connected weights ones.
std::vector<WeightsHeader> headers();

/// @brief Every weights layer kind with the combinations of it's options and
/// every layer kind without the weights between the weights layers.
std::vector<WeightsVariant> layers();

/// @brief The network families with the resolution dependent and the
/// resolution independent weights at every resolution, 1 and 3 channels.
std::vector<WeightsVariant> resolutions_sweep();

/**
 * @brief The random networks of the random layers with the random options.
 *
 * The DARKNETXX_WEIGHTS_TESTS_SEED and the DARKNETXX_WEIGHTS_TESTS_RANDOM_COUNT
 * environment variables replace the given seed and count, which generates
 * as many other variants as needed.
 */
std::vector<WeightsVariant> random(uint64_t seed, size_t count);

/// @brief The real world sized networks: the tens and hundreds of megabytes
/// weights files.
std::vector<WeightsVariant> big();

/// @brief Wraps the network into the variant with the name derived seeds.
WeightsVariant make(const std::string& name, const NetSpec& spec,
                    ParseMode mode, const WeightsHeader& header);

}  // namespace variants

}  // namespace weights_factory

#endif  // DARKNETXX_PROJECT_WEIGHTS_FACTORY_VARIANTS_H
