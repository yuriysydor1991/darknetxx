#ifndef DARKNET_CXX_PORT_PROJECT_DARKNETSTRINGS_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_DARKNETSTRINGS_CLASS_H

#include <memory>
#include <string>

namespace darknet_adaptor
{

/**
 * @brief Class to encapsulate component related strings.
 */
class DarknetStrings
{
 public:
  virtual ~DarknetStrings() = default;
  DarknetStrings() = default;

  inline static const std::string detector = "detector";
};

}  // namespace darknet_adaptor

#endif  // DARKNET_CXX_PORT_PROJECT_DARKNETSTRINGS_CLASS_H
