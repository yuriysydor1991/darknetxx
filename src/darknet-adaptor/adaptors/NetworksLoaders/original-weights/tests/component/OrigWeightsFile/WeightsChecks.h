#ifndef DARKNETXX_PROJECT_CTEST_ORIG_WEIGHTS_FILE_CHECKS_H
#define DARKNETXX_PROJECT_CTEST_ORIG_WEIGHTS_FILE_CHECKS_H

#include <gtest/gtest.h>

#include <cstdint>
#include <string>

#include "DarknetNetwork.h"
#include "NetworkChecks.h"
#include "WeightsFile.h"
#include "WeightsNetSpec.h"
#include "WeightsVariants.h"
#include "WeightsWorkspace.h"

namespace weights_test
{

// the generated networks and weights files of the weights factory
using namespace weights_factory;

/// @brief The OrigWeightsWriter save() and the OrigWeightsReader load() of
/// the network, expected to succeed.
void save(network& net, const std::string& path);
void save_upto(network& net, const std::string& path, int cutoff, bool ema);
void load(network& net, const std::string& path);
void load_upto(network& net, const std::string& path, int cutoff);

/// @brief The save_weights() file of the pattern filled network holds the
/// header and every array in the model order, bit by bit.
void check_save(const WeightsVariant& variant,
                const WeightsWorkspace& workspace);

/// @brief The load_weights() of the independently written file of every
/// header version puts every value to it's array, the loader transpositions
/// applied, and leaves the dontload layers intact.
void check_load(const WeightsVariant& variant,
                const WeightsWorkspace& workspace);

/// @brief The saved network loads bit exact into a new one, which saves the
/// same bytes again.
void check_round_trip(const WeightsVariant& variant,
                      const WeightsWorkspace& workspace);

/// @brief The truncated file loads the whole floats it holds and leaves the
/// rest of the arrays intact.
void check_truncated(const WeightsVariant& variant,
                     const WeightsWorkspace& workspace);

/// @brief The save_weights_upto() and the load_weights_upto() cutoffs,
/// including the cut files loaded by the load_weights().
void check_cutoff(const WeightsVariant& variant,
                  const WeightsWorkspace& workspace);

/// @brief The save_weights_upto(..., save_ema = 1) file holds the EMA arrays
/// of the convolutional layers and the regular arrays of the rest.
void check_ema(const WeightsVariant& variant,
               const WeightsWorkspace& workspace);

}  // namespace weights_test

#endif  // DARKNETXX_PROJECT_CTEST_ORIG_WEIGHTS_FILE_CHECKS_H
