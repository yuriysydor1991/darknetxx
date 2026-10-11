#include "src/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz3/v3/Dxxwjz3CfgJson.h"

#include <cstddef>
#include <exception>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include "src/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz1/v1/Dxxwjz1Format.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz1/v1/Dxxwjz1JsonString.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz3/v3/Dxxwjz3Format.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz3/v3/Dxxwjz3ValueCodec.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/weights-model/WeightsSchema.h"

namespace darknet_adaptor::loaders::dxxwjz3::v3
{

using dxxwjz1::v1::Dxxwjz1JsonString;

namespace
{

/// @brief The JSON text of the member key: "\"name\": ".
std::string key(const std::string& name)
{
  return Dxxwjz1JsonString::quote(name) + ": ";
}

}  // namespace

Dxxwjz3CfgJson::Dxxwjz3CfgJson(const std::string& cfg)
{
  std::istringstream text{cfg};
  CfgSections sections;

  try {
    sections = CfgSection::read_cfg(text, "the network cfg text");
  }
  catch (const std::exception&) {
    return;
  }

  if (sections.empty() || (sections.front().type() != "[net]" &&
                           sections.front().type() != "[network]")) {
    return;
  }

  for (std::size_t index = 1U; index < sections.size(); ++index) {
    const std::string& header = sections[index].type();
    const std::string inner = options(sections[index]);

    if (header.size() < 3U || header.back() != ']') {
      return;
    }

    mlayers.push_back(
        key(dxxwjz1::v1::Dxxwjz1Format::TYPE_KEY) +
        Dxxwjz1JsonString::quote(header.substr(1U, header.size() - 2U)) +
        (inner.empty()
             ? ""
             : ", " + key(Dxxwjz3Format::OPTIONS_KEY) + "{" + inner + "}"));
    mtypes.push_back(WeightsSchema::section_type(header));
  }

  mnet = options(sections.front());
  mvalid = true;
}

bool Dxxwjz3CfgJson::valid() const { return mvalid; }

const std::string& Dxxwjz3CfgJson::net() const { return mnet; }

const std::vector<std::string>& Dxxwjz3CfgJson::layers() const
{
  return mlayers;
}

const std::vector<LAYER_TYPE>& Dxxwjz3CfgJson::types() const { return mtypes; }

std::string Dxxwjz3CfgJson::options(const CfgSection& section)
{
  std::set<std::string> keys;
  std::string json;

  for (const auto& [name, value] : section.options()) {
    if (keys.insert(name).second) {
      json += (json.empty() ? "" : ", ") + key(name) +
              Dxxwjz3ValueCodec::json_of(value);
    }
  }

  return json;
}

}  // namespace darknet_adaptor::loaders::dxxwjz3::v3
