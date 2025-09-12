#ifndef DARKNETCXX_PROJECT_STRINGTOOLS_HELPER_CLASS_H
#define DARKNETCXX_PROJECT_STRINGTOOLS_HELPER_CLASS_H

#include <sstream>
#include <string>
#include <vector>

namespace helpers
{

/**
 * @brief The string tools helper class.
 */
class StringTools
{
 public:
  using svector = std::vector<std::string>;

  virtual ~StringTools() = default;
  StringTools() = default;

  static std::vector<std::string> split(const std::string& s,
                                        const char delimiter);
};

}  // namespace helpers

#endif  // DARKNETCXX_PROJECT_STRINGTOOLS_HELPER_CLASS_H
