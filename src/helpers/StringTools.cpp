#include "src/helpers/StringTools.h"

#include <algorithm>
#include <sstream>
#include <string>
#include <vector>

namespace helpers
{

std::vector<std::string> StringTools::split(const std::string& s,
                                            char delimiter)
{
  svector tokens;

  const auto reserve = std::count(s.begin(), s.end(), delimiter) + 1U;

  tokens.reserve(static_cast<svector::size_type>(reserve));

  std::stringstream ss(s);

  std::string item;

  while (std::getline(ss, item, delimiter)) {
    tokens.emplace_back(item);
  }

  return tokens;
}

}  // namespace helpers