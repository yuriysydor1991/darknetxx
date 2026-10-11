#include "src/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz3/v3/Dxxwjz3MetadataReader.h"

#include <cstddef>
#include <numeric>
#include <string>
#include <utility>
#include <vector>

#include "src/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz1/v1/Dxxwjz1Format.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz2/v2/Dxxwjz2Format.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz3/v3/Dxxwjz3Format.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz3/v3/Dxxwjz3ValueCodec.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/weights-model/WeightsSchema.h"

namespace darknet_adaptor::loaders::dxxwjz3::v3
{

using dxxwjz1::v1::Dxxwjz1Format;
using dxxwjz2::v2::Dxxwjz2Format;

namespace
{

/// @brief The white space the darknet drops of the cfg lines.
constexpr const char* const SPACES = " \t\r\n";

}  // namespace

Dxxwjz3MetadataReader::Dxxwjz3MetadataReader(WeightsReport& nreport)
    : Dxxwjz2MetadataReader{nreport}
{
}

bool Dxxwjz3MetadataReader::network(const json& value,
                                    NetworkDescription& description)
{
  const std::string path{Dxxwjz2Format::NETWORK_KEY};
  const std::string layers_path = member(path, Dxxwjz1Format::LAYERS_KEY);
  std::string cfg;
  std::vector<std::string> names;

  if (!object(value, path, {Dxxwjz3Format::NET_KEY, Dxxwjz1Format::LAYERS_KEY},
              {Dxxwjz3Format::NET_KEY, Dxxwjz1Format::LAYERS_KEY,
               Dxxwjz2Format::CLASSES_KEY}) ||
      !section("net", value.at(Dxxwjz3Format::NET_KEY),
               member(path, Dxxwjz3Format::NET_KEY), cfg)) {
    return false;
  }

  const json& layers = value.at(Dxxwjz1Format::LAYERS_KEY);

  if (!layers.is_array()) {
    return fail(layers_path + " isn't an array");
  }

  for (std::size_t index = 0U; index < layers.size(); ++index) {
    if (!layer(layers[index], layers_path + "[" + std::to_string(index) + "]",
               cfg)) {
      return false;
    }
  }

  if (value.contains(Dxxwjz2Format::CLASSES_KEY) &&
      !names_of(value.at(Dxxwjz2Format::CLASSES_KEY), names)) {
    return false;
  }

  description = NetworkDescription{cfg, names};

  return true;
}

bool Dxxwjz3MetadataReader::training(const json& value, TrainingRecord& record)
{
  const std::string path{Dxxwjz2Format::TRAINING_KEY};
  double time = 0.0;
  std::vector<float> losses;
  std::vector<float> avg_losses;

  if (!object(value, path,
              {Dxxwjz2Format::SECONDS_KEY, Dxxwjz2Format::LOSS_KEY,
               Dxxwjz2Format::AVG_LOSS_KEY},
              {Dxxwjz2Format::SECONDS_KEY, Dxxwjz2Format::LOSS_KEY,
               Dxxwjz2Format::AVG_LOSS_KEY, Dxxwjz2Format::MAP_ITERATION_KEY,
               Dxxwjz2Format::MAP_KEY}) ||
      !seconds(value.at(Dxxwjz2Format::SECONDS_KEY), time) ||
      !floats(value.at(Dxxwjz2Format::LOSS_KEY),
              member(path, Dxxwjz2Format::LOSS_KEY), losses) ||
      !floats(value.at(Dxxwjz2Format::AVG_LOSS_KEY),
              member(path, Dxxwjz2Format::AVG_LOSS_KEY), avg_losses)) {
    return false;
  }

  // the losses of the iterations of their indexes
  std::vector<int> iterations(losses.size());

  std::iota(iterations.begin(), iterations.end(), 1);

  if (!record.assign(std::move(iterations), std::move(losses),
                     std::move(avg_losses))) {
    return fail("the " + member(path, Dxxwjz2Format::LOSS_KEY) + " and the " +
                member(path, Dxxwjz2Format::AVG_LOSS_KEY) +
                " columns are of the different sizes");
  }

  record.seconds(time);

  return maps(value, record);
}

bool Dxxwjz3MetadataReader::arrays(json& value, const int index,
                                   std::vector<WeightsLayer>& layers)
{
  const std::string path =
      member(Dxxwjz2Format::NETWORK_KEY, Dxxwjz1Format::LAYERS_KEY) + "[" +
      std::to_string(index) + "]";
  const auto flat = value.find(Dxxwjz1Format::ARRAYS_KEY);
  const auto nested = value.find(Dxxwjz1Format::SUBLAYERS_KEY);
  const auto type = value.find(Dxxwjz1Format::TYPE_KEY);
  WeightsLayer layer{index, BLANK, {}};

  if (flat == value.end() && nested == value.end()) {
    return true;
  }

  if (type == value.end() || !type->is_string() ||
      (layer.type = WeightsSchema::section_type("[" + type->get<std::string>() +
                                                "]")) == BLANK) {
    return fail(member(path, Dxxwjz1Format::TYPE_KEY) +
                " isn't the darknet layer type of the arrays");
  }

  if ((flat != value.end() &&
       !arrays_of(*flat, member(path, Dxxwjz1Format::ARRAYS_KEY),
                  WeightsSubLayer::None, layer.arrays)) ||
      (nested != value.end() &&
       !sublayers_of(*nested, member(path, Dxxwjz1Format::SUBLAYERS_KEY),
                     layer.arrays))) {
    return false;
  }

  // the tree of the network keeps no weights
  value.erase(Dxxwjz1Format::ARRAYS_KEY);
  value.erase(Dxxwjz1Format::SUBLAYERS_KEY);
  layers.push_back(std::move(layer));

  return true;
}

bool Dxxwjz3MetadataReader::layer(const json& value, const std::string& path,
                                  std::string& cfg)
{
  if (!object(value, path, {Dxxwjz1Format::TYPE_KEY},
              {Dxxwjz1Format::TYPE_KEY, Dxxwjz3Format::OPTIONS_KEY})) {
    return false;
  }

  const json& type = value.at(Dxxwjz1Format::TYPE_KEY);

  // the [type] line of the section
  if (!type.is_string() || type.get_ref<const std::string&>().empty() ||
      type.get_ref<const std::string&>().find_first_of(
          std::string{SPACES} + "[]") != std::string::npos) {
    return fail(member(path, Dxxwjz1Format::TYPE_KEY) +
                " isn't the name of a cfg section");
  }

  return section(type.get<std::string>(),
                 value.contains(Dxxwjz3Format::OPTIONS_KEY)
                     ? value.at(Dxxwjz3Format::OPTIONS_KEY)
                     : json::object(),
                 member(path, Dxxwjz3Format::OPTIONS_KEY), cfg);
}

bool Dxxwjz3MetadataReader::names_of(const json& value,
                                     std::vector<std::string>& names)
{
  const std::string path =
      member(Dxxwjz2Format::NETWORK_KEY, Dxxwjz2Format::CLASSES_KEY);

  if (!value.is_array()) {
    return fail(path + " isn't an array");
  }

  for (const json& name : value) {
    if (!name.is_string()) {
      return fail(path + "[" + std::to_string(names.size()) +
                  "] isn't a string");
    }

    names.push_back(name.get<std::string>());
  }

  return true;
}

bool Dxxwjz3MetadataReader::section(const std::string& type,
                                    const json& options,
                                    const std::string& path, std::string& cfg)
{
  if (!options.is_object()) {
    return fail(path + " isn't an object");
  }

  cfg += (cfg.empty() ? "[" : "\n[") + type + "]\n";

  for (const auto& option : options.items()) {
    const std::string& name = option.key();
    const bool null = option.value().is_null();
    std::string text;

    // the key of the key=value line, which the darknet splits at it's first
    // '=', of no comment or section start
    if (name.empty() ||
        name.find_first_of(std::string{SPACES} + "=") != std::string::npos ||
        name.find_first_of("#;[") == 0U) {
      return fail(member(path, name.c_str()) +
                  " isn't the key of a cfg option");
    }

    if (!null && !Dxxwjz3ValueCodec::text_of(option.value(), text)) {
      return fail(member(path, name.c_str()) +
                  " isn't a cfg option value: a number, a string of no white "
                  "space, the array of them or the null");
    }

    // the null of the key alone, of no '=' (the NULL value of the darknet)
    cfg += null ? name + "\n" : name + "=" + text + "\n";
  }

  return true;
}

bool Dxxwjz3MetadataReader::sublayers_of(const json& value,
                                         const std::string& path,
                                         std::vector<WeightsArray>& arrays)
{
  if (!value.is_object()) {
    return fail(path + " isn't an object");
  }

  for (const auto& item : value.items()) {
    const std::string where = member(path, item.key().c_str());
    const auto found = item.value().find(Dxxwjz1Format::ARRAYS_KEY);
    WeightsSubLayer sub = WeightsSubLayer::None;

    if (!WeightsSchema::parse(item.key(), sub)) {
      return fail(where + " isn't a darknet sub-layer name");
    }

    if (!object(item.value(), where, {}, {Dxxwjz1Format::ARRAYS_KEY}) ||
        (found != item.value().end() &&
         !arrays_of(*found, member(where, Dxxwjz1Format::ARRAYS_KEY), sub,
                    arrays))) {
      return false;
    }
  }

  return true;
}

bool Dxxwjz3MetadataReader::arrays_of(const json& value,
                                      const std::string& path,
                                      const WeightsSubLayer sub,
                                      std::vector<WeightsArray>& arrays)
{
  if (!value.is_object()) {
    return fail(path + " isn't an object");
  }

  for (const auto& item : value.items()) {
    const std::string where = member(path, item.key().c_str());
    WeightsArray array;

    array.sub = sub;

    if (!WeightsSchema::parse(item.key(), array.kind)) {
      return fail(where + " isn't a darknet weights array name");
    }

    if (!floats(item.value(), where, array.values)) {
      return false;
    }

    arrays.push_back(std::move(array));
  }

  return true;
}

}  // namespace darknet_adaptor::loaders::dxxwjz3::v3
