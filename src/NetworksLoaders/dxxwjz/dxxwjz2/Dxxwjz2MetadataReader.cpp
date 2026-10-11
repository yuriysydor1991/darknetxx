#include "src/NetworksLoaders/dxxwjz/dxxwjz2/Dxxwjz2MetadataReader.h"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <initializer_list>
#include <limits>
#include <string>
#include <vector>

#include "src/NetworksLoaders/dxxwjz/dxxwjz1/Dxxwjz1FloatCodec.h"
#include "src/NetworksLoaders/dxxwjz/dxxwjz1/Dxxwjz1Format.h"
#include "src/NetworksLoaders/dxxwjz/dxxwjz2/Dxxwjz2Format.h"

namespace darknet_adaptor::loaders::dxxwjz2::v2
{

using dxxwjz1::v1::Dxxwjz1FloatCodec;
using dxxwjz1::v1::Dxxwjz1Format;

Dxxwjz2MetadataReader::Dxxwjz2MetadataReader(WeightsReport& nreport)
    : mreport{nreport}
{
}

bool Dxxwjz2MetadataReader::network(const json& value,
                                    NetworkDescription& description)
{
  const std::string path{Dxxwjz2Format::NETWORK_KEY};

  if (!object(value, path, {Dxxwjz2Format::CFG_KEY},
              {Dxxwjz2Format::CFG_KEY, Dxxwjz2Format::CLASSES_KEY})) {
    return false;
  }

  const json& cfg = value.at(Dxxwjz2Format::CFG_KEY);

  if (!cfg.is_string() || cfg.get_ref<const std::string&>().empty()) {
    return fail(member(path, Dxxwjz2Format::CFG_KEY) +
                " isn't the text of the network cfg file");
  }

  std::vector<std::string> names;

  if (value.contains(Dxxwjz2Format::CLASSES_KEY) &&
      !classes(value.at(Dxxwjz2Format::CLASSES_KEY), names)) {
    return false;
  }

  description = NetworkDescription{cfg.get<std::string>(), names};

  return true;
}

bool Dxxwjz2MetadataReader::training(const json& value, TrainingRecord& record)
{
  const std::string path{Dxxwjz2Format::TRAINING_KEY};

  if (!object(value, path,
              {Dxxwjz2Format::SECONDS_KEY, Dxxwjz2Format::ITERATION_KEY,
               Dxxwjz2Format::LOSS_KEY, Dxxwjz2Format::AVG_LOSS_KEY},
              {Dxxwjz2Format::SECONDS_KEY, Dxxwjz2Format::ITERATION_KEY,
               Dxxwjz2Format::LOSS_KEY, Dxxwjz2Format::AVG_LOSS_KEY,
               Dxxwjz2Format::MAP_ITERATION_KEY, Dxxwjz2Format::MAP_KEY})) {
    return false;
  }

  double time = 0.0;
  std::vector<int> iterations;
  std::vector<float> losses;
  std::vector<float> avg_losses;

  if (!seconds(value.at(Dxxwjz2Format::SECONDS_KEY), time) ||
      !integers(value.at(Dxxwjz2Format::ITERATION_KEY),
                member(path, Dxxwjz2Format::ITERATION_KEY), iterations) ||
      !floats(value.at(Dxxwjz2Format::LOSS_KEY),
              member(path, Dxxwjz2Format::LOSS_KEY), losses) ||
      !floats(value.at(Dxxwjz2Format::AVG_LOSS_KEY),
              member(path, Dxxwjz2Format::AVG_LOSS_KEY), avg_losses)) {
    return false;
  }

  if (!record.assign(iterations, losses, avg_losses)) {
    return fail("the " + member(path, Dxxwjz2Format::ITERATION_KEY) + ", the " +
                member(path, Dxxwjz2Format::LOSS_KEY) + " and the " +
                member(path, Dxxwjz2Format::AVG_LOSS_KEY) +
                " columns are of the different sizes");
  }

  record.seconds(time);

  return maps(value, record);
}

const std::string& Dxxwjz2MetadataReader::error() const { return merror; }

bool Dxxwjz2MetadataReader::maps(const json& value, TrainingRecord& record)
{
  const std::string iterations_path =
      member(Dxxwjz2Format::TRAINING_KEY, Dxxwjz2Format::MAP_ITERATION_KEY);
  const std::string maps_path =
      member(Dxxwjz2Format::TRAINING_KEY, Dxxwjz2Format::MAP_KEY);
  const bool given = value.contains(Dxxwjz2Format::MAP_ITERATION_KEY);
  std::vector<int> iterations;
  std::vector<float> values;

  if (given != value.contains(Dxxwjz2Format::MAP_KEY)) {
    return fail("the " + iterations_path + " and the " + maps_path +
                " columns go together");
  }

  if (!given) {
    return true;
  }

  if (!integers(value.at(Dxxwjz2Format::MAP_ITERATION_KEY), iterations_path,
                iterations) ||
      !floats(value.at(Dxxwjz2Format::MAP_KEY), maps_path, values)) {
    return false;
  }

  if (std::adjacent_find(iterations.begin(), iterations.end(),
                         std::greater_equal<>{}) != iterations.end()) {
    return fail(iterations_path + " isn't of the ascending iterations");
  }

  if (!record.assign_maps(iterations, values)) {
    return fail("the " + iterations_path + " and the " + maps_path +
                " columns are of the different sizes");
  }

  return true;
}

bool Dxxwjz2MetadataReader::classes(const json& value,
                                    std::vector<std::string>& names)
{
  const std::string path = std::string{Dxxwjz2Format::NETWORK_KEY} + "." +
                           Dxxwjz2Format::CLASSES_KEY;

  if (!value.is_array()) {
    return fail(member(Dxxwjz2Format::NETWORK_KEY, Dxxwjz2Format::CLASSES_KEY) +
                " isn't an array");
  }

  names.assign(value.size(), std::string{});
  std::vector<bool> given(value.size(), false);

  for (std::size_t position = 0U; position < value.size(); ++position) {
    if (!class_of(value[position], path + "[" + std::to_string(position) + "]",
                  names, given)) {
      return false;
    }
  }

  return true;
}

bool Dxxwjz2MetadataReader::class_of(const json& value, const std::string& path,
                                     std::vector<std::string>& names,
                                     std::vector<bool>& given)
{
  const std::initializer_list<const char*> keys{Dxxwjz1Format::INDEX_KEY,
                                                Dxxwjz2Format::NAME_KEY};

  if (!object(value, path, keys, keys)) {
    return false;
  }

  const json& index = value.at(Dxxwjz1Format::INDEX_KEY);
  const json& name = value.at(Dxxwjz2Format::NAME_KEY);

  // the indexes from zero of every class once
  if (!index.is_number_unsigned() || index.get<uint64_t>() >= names.size()) {
    return fail(member(path, Dxxwjz1Format::INDEX_KEY) +
                " isn't an index of the " + std::to_string(names.size()) +
                " classes");
  }

  if (!name.is_string()) {
    return fail(member(path, Dxxwjz2Format::NAME_KEY) + " isn't a string");
  }

  const std::size_t at = index.get<std::size_t>();

  if (given[at]) {
    return fail(member(path, Dxxwjz1Format::INDEX_KEY) + " " +
                std::to_string(at) + " is given twice");
  }

  given[at] = true;
  names[at] = name.get<std::string>();

  return true;
}

bool Dxxwjz2MetadataReader::seconds(const json& value, double& result)
{
  if (!value.is_number() || !std::isfinite(value.get<double>()) ||
      value.get<double>() < 0.0) {
    return fail(
        member(Dxxwjz2Format::TRAINING_KEY, Dxxwjz2Format::SECONDS_KEY) +
        " isn't a count of the seconds");
  }

  result = value.get<double>();

  return true;
}

bool Dxxwjz2MetadataReader::integers(const json& value, const std::string& path,
                                     std::vector<int>& column)
{
  if (!value.is_array()) {
    return fail(path + " isn't an array");
  }

  column.reserve(value.size());

  for (const json& element : value) {
    const bool fits =
        element.is_number_unsigned()
            ? element.get<uint64_t>() <=
                  static_cast<uint64_t>(std::numeric_limits<int>::max())
            : element.is_number_integer() &&
                  element.get<int64_t>() >= std::numeric_limits<int>::min();

    if (!fits) {
      return fail(path + "[" + std::to_string(column.size()) +
                  "] isn't an iteration number");
    }

    column.push_back(element.get<int>());
  }

  return true;
}

bool Dxxwjz2MetadataReader::floats(const json& value, const std::string& path,
                                   std::vector<float>& column)
{
  if (!value.is_array()) {
    return fail(path + " isn't an array");
  }

  column.reserve(value.size());

  for (const json& element : value) {
    float number = 0.f;

    // the exact floats of the tree and the bit patterns of the non finite
    if (element.is_number()) {
      number = static_cast<float>(element.get<double>());
    } else if (!element.is_string() ||
               !Dxxwjz1FloatCodec::decode_bits(
                   element.get_ref<const std::string&>(), number)) {
      return fail(path + "[" + std::to_string(column.size()) +
                  "] isn't a float");
    }

    column.push_back(number);
  }

  return true;
}

bool Dxxwjz2MetadataReader::object(const json& value, const std::string& path,
                                   std::initializer_list<const char*> required,
                                   std::initializer_list<const char*> known)
{
  if (!value.is_object()) {
    return fail(path + " isn't an object");
  }

  for (const char* const name : required) {
    if (!value.contains(name)) {
      return fail(path + " has no \"" + name + "\" key");
    }
  }

  for (const auto& item : value.items()) {
    const bool is_known =
        std::any_of(known.begin(), known.end(),
                    [&item](const char* name) { return item.key() == name; });

    if (!is_known) {
      mreport.warning("the unknown key " + member(path, item.key().c_str()) +
                      " is skipped");
    }
  }

  return true;
}

std::string Dxxwjz2MetadataReader::member(const std::string& path,
                                          const char* const name)
{
  return path + ".\"" + name + "\"";
}

bool Dxxwjz2MetadataReader::fail(const std::string& message)
{
  if (merror.empty()) {
    merror = message;
  }

  return false;
}

}  // namespace darknet_adaptor::loaders::dxxwjz2::v2
