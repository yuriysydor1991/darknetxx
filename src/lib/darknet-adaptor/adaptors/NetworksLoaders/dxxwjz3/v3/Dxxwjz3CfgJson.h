#ifndef DARKNET_CXX_PORT_PROJECT_DXXWJZ3_V3_CFG_JSON_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_DXXWJZ3_V3_CFG_JSON_CLASS_H

#include <string>
#include <vector>

#include "src/darknet.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/OrigDefaultLoader/CfgSection.h"

namespace darknet_adaptor::loaders::dxxwjz3::v3
{

/**
 * @brief The JSON text of the network cfg of the dxxwjz3 files (see the
 * Dxxwjz3Format) of the cfg text read the way the darknet reads it (see the
 * CfgSection::read_cfg()): the options of the [net] section and the members
 * of the layer sections.
 */
class Dxxwjz3CfgJson
{
 public:
  /// @param cfg The cfg text; the one the JSON can't keep (of no [net]
  /// section first, of an option before it, of a section header of no
  /// closing bracket) makes the JSON of no valid().
  explicit Dxxwjz3CfgJson(const std::string& cfg);

  /// @brief Tells the JSON keeps the cfg.
  bool valid() const;

  /// @brief The members of the options of the [net] section.
  const std::string& net() const;

  /// @brief The type and the options members of every layer section in the
  /// cfg order, of no options member for the section of no options.
  const std::vector<std::string>& layers() const;

  /// @brief The layer types of the layer sections in the cfg order (see the
  /// WeightsSchema::section_type()).
  const std::vector<LAYER_TYPE>& types() const;

 private:
  /// @brief The members of the options of the section, the first one of the
  /// repeated keys (the one the darknet reads).
  static std::string options(const CfgSection& section);

  bool mvalid{false};
  std::string mnet;
  std::vector<std::string> mlayers;
  std::vector<LAYER_TYPE> mtypes;
};

}  // namespace darknet_adaptor::loaders::dxxwjz3::v3

#endif  // DARKNET_CXX_PORT_PROJECT_DXXWJZ3_V3_CFG_JSON_CLASS_H
