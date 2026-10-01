#include "WeightsWorkspace.h"

#include <gtest/gtest.h>

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <random>
#include <string>
#include <system_error>

namespace weights_factory
{

namespace
{

/// @brief The per process token, which keeps the files of the tests running
/// in parallel apart.
const std::string& process_token()
{
  static const std::string token = std::to_string(std::random_device{}());
  return token;
}

}  // namespace

WeightsWorkspace::WeightsWorkspace(const std::string& root)
{
  const ::testing::TestInfo* const info =
      ::testing::UnitTest::GetInstance()->current_test_info();

  std::string name = info == nullptr ? std::string{"weights"}
                                     : std::string{info->test_suite_name()} +
                                           "." + info->name();

  std::replace_if(
      name.begin(), name.end(),
      [](char symbol) {
        return std::isalnum(static_cast<unsigned char>(symbol)) == 0 &&
               symbol != '_' && symbol != '.' && symbol != '-';
      },
      '_');

  const char* const keep = std::getenv("DARKNETXX_WEIGHTS_TESTS_KEEP_DIR");

  std::filesystem::path path;

  if (keep != nullptr && *keep != '\0') {
    mkeep = true;
    path = std::filesystem::path{keep} / name;
  } else {
    path = std::filesystem::path{root} / (process_token() + "-" + name);
  }

  mdir = path.string();

  std::filesystem::create_directories(path);
}

WeightsWorkspace::~WeightsWorkspace()
{
  if (!mkeep) {
    std::error_code error;
    std::filesystem::remove_all(mdir, error);
  }
}

std::string WeightsWorkspace::file(const std::string& name) const
{
  return (std::filesystem::path{mdir} / name).string();
}

std::string WeightsWorkspace::write_cfg(const NetSpec& spec) const
{
  const std::string path =
      file((spec.name.empty() ? "net" : spec.name) + ".cfg");

  std::ofstream cfg(path, std::ios::trunc);
  cfg << spec.cfg();

  return path;
}

const std::string& WeightsWorkspace::dir() const { return mdir; }

}  // namespace weights_factory
