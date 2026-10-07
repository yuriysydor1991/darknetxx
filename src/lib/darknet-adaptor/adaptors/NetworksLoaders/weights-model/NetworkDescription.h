#ifndef DARKNET_CXX_PORT_PROJECT_NETWORK_DESCRIPTION_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_NETWORK_DESCRIPTION_CLASS_H

#include <string>
#include <vector>

namespace darknet_adaptor::loaders
{

/**
 * @brief The network the weights are of: the text of its cfg file and the
 * names of the classes it detects, the name of a class at the class index.
 */
class NetworkDescription
{
 public:
  virtual ~NetworkDescription() = default;
  NetworkDescription() = default;
  NetworkDescription(std::string ncfg, std::vector<std::string> nclasses);

  const std::string& cfg() const;
  void cfg(std::string ncfg);

  const std::vector<std::string>& classes() const;
  void classes(std::vector<std::string> nclasses);

  /// @brief Tells there is no cfg text.
  bool empty() const;

 private:
  std::string mcfg;
  std::vector<std::string> mclasses;
};

}  // namespace darknet_adaptor::loaders

#endif  // DARKNET_CXX_PORT_PROJECT_NETWORK_DESCRIPTION_CLASS_H
