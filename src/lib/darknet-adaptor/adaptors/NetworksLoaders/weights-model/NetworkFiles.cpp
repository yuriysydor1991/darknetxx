#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/weights-model/NetworkFiles.h"

#include <fstream>
#include <iterator>
#include <string>
#include <vector>

namespace darknet_adaptor::loaders
{

bool NetworkFiles::read_text(const std::string& path, std::string& text)
{
  std::ifstream file{path, std::ios::binary};

  if (!file.is_open()) {
    return false;
  }

  text.assign(std::istreambuf_iterator<char>{file},
              std::istreambuf_iterator<char>{});

  return !file.bad();
}

bool NetworkFiles::read_classes(const std::string& path,
                                std::vector<std::string>& classes)
{
  std::ifstream file{path, std::ios::binary};

  if (!file.is_open()) {
    return false;
  }

  classes.clear();

  for (std::string line; std::getline(file, line);) {
    if (!line.empty() && line.back() == '\r') {
      line.pop_back();
    }

    classes.push_back(line);
  }

  return !file.bad();
}

}  // namespace darknet_adaptor::loaders
