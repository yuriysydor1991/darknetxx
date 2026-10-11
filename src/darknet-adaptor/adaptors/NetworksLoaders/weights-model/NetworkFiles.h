#ifndef DARKNET_CXX_PORT_PROJECT_NETWORK_FILES_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_NETWORK_FILES_CLASS_H

#include <string>
#include <vector>

namespace darknet_adaptor::loaders
{

/// @brief Reads the files a network is described by: the cfg file and the
/// class names one.
class NetworkFiles
{
 public:
  static bool read_text(const std::string& path, std::string& text);

  /**
   * @brief The names of the darknet class names file: a name on a line, the
   * CR LF line ends included, the class index of a name is its line number
   * from zero.
   *
   * @return False for the file unable to be read.
   */
  static bool read_classes(const std::string& path,
                           std::vector<std::string>& classes);
};

}  // namespace darknet_adaptor::loaders

#endif  // DARKNET_CXX_PORT_PROJECT_NETWORK_FILES_CLASS_H
