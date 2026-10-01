#ifndef DARKNETXX_PROJECT_ITEST_CONVERTER_RUNS_H
#define DARKNETXX_PROJECT_ITEST_CONVERTER_RUNS_H

#include <gtest/gtest.h>

#include <string>
#include <vector>

#include "DarknetNetwork.h"
#include "WeightsNetSpec.h"
#include "WeightsVariants.h"
#include "WeightsWorkspace.h"

namespace converter_test
{

// the generated networks and weights files of the weights factory
using namespace weights_factory;

/// @brief The output and the exit status of an executable run.
struct RunResult
{
  /// the exit status, -1 for the process which didn't exit on its own
  int status{-1};
  /// the path of the file of the standard output and error of the run
  std::string log;

  /// @brief The text of the log.
  std::string output() const;
};

/**
 * @brief Runs the executable with the arguments through the shell, the
 * output of the run goes into the log file.
 */
RunResult run(const std::string& executable,
              const std::vector<std::string>& args, const std::string& log);

/// @brief The conversion arguments: the source and the destination with
/// their formats, the network cfg for the not empty one.
std::vector<std::string> conversion_args(const std::string& from,
                                         const std::string& src,
                                         const std::string& to,
                                         const std::string& dst,
                                         const std::string& cfg);

/// @brief Tells the gzip file by its magic bytes.
bool is_gzip(const std::string& path);

/**
 * @brief Converts the original weights file of the network to the dxxwjz1 one
 * (the file of the weights one path and the .dxxwjz1 extension) and back with
 * the executable: the dxxwjz1 file is the gzip one, which converts back into
 * the same bytes with and without the network cfg and into itself.
 */
void check_back_and_forth(const std::string& executable,
                          const std::string& weights, const std::string& cfg,
                          const WeightsWorkspace& workspace);

/**
 * @brief The files of the variant convert back and forth with the converter
 * binary (see the check_back_and_forth()): the file darknet saves and the
 * file of the variant header made for the load, the dxxwjz1 files load the
 * networks the weights files do.
 */
void check_variant(const WeightsVariant& variant,
                   const WeightsWorkspace& workspace);

}  // namespace converter_test

#endif  // DARKNETXX_PROJECT_ITEST_CONVERTER_RUNS_H
