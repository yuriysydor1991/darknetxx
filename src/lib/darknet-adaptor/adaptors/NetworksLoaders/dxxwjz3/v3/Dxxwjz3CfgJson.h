#ifndef DARKNET_CXX_PORT_PROJECT_DXXWJZ3_V3_CFG_JSON_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_DXXWJZ3_V3_CFG_JSON_CLASS_H

#include <string>

#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/OrigDefaultLoader/CfgSection.h"

namespace darknet_adaptor::loaders::dxxwjz3::v3
{

/// @brief The JSON text of the network cfg of the dxxwjz3 files (see the
/// Dxxwjz3Format).
class Dxxwjz3CfgJson
{
 public:
  /**
   * @brief The net and the layers members of the cfg text read the way the
   * darknet reads it (see the CfgSection::read_cfg()), a layer on a line.
   *
   * @return The empty text for the cfg the members can't keep: the one of no
   * [net] section first, of an option before it, of a section header of no
   * closing bracket.
   */
  static std::string members(const std::string& cfg);

 private:
  /// @brief The members of the options of the section, the first one of the
  /// repeated keys (the one the darknet reads).
  static std::string options(const CfgSection& section);
};

}  // namespace darknet_adaptor::loaders::dxxwjz3::v3

#endif  // DARKNET_CXX_PORT_PROJECT_DXXWJZ3_V3_CFG_JSON_CLASS_H
