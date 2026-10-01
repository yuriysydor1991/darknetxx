#ifndef DARKNETXX_PROJECT_WEIGHTS_FACTORY_WORKSPACE_H
#define DARKNETXX_PROJECT_WEIGHTS_FACTORY_WORKSPACE_H

#include <string>

#include "WeightsNetSpec.h"

namespace weights_factory
{

/**
 * @brief The directory of the files of the current test, removed with them at
 * the test end.
 *
 * The directory is made inside the given root one of the test executable. The
 * DARKNETXX_WEIGHTS_TESTS_KEEP_DIR environment variable names the directory to
 * keep the generated cfg and weights files in instead, one sub-directory per
 * test.
 */
class WeightsWorkspace
{
 public:
  explicit WeightsWorkspace(const std::string& root);
  ~WeightsWorkspace();

  WeightsWorkspace(const WeightsWorkspace&) = delete;
  WeightsWorkspace& operator=(const WeightsWorkspace&) = delete;

  /// @brief The path of the named file in the workspace.
  std::string file(const std::string& name) const;

  /// @brief Writes the cfg file of the network, returns the file path.
  std::string write_cfg(const NetSpec& spec) const;

  /// @brief The workspace directory.
  const std::string& dir() const;

 private:
  std::string mdir;
  bool mkeep{false};
};

}  // namespace weights_factory

#endif  // DARKNETXX_PROJECT_WEIGHTS_FACTORY_WORKSPACE_H
