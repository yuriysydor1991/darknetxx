#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/OrigDefaultLoader/CfgSection.h"

#include <cstdlib>
#include <fstream>
#include <string>
#include <utility>
#include <vector>

#include "src/lib/darknet-adaptor/adaptors/NetworkError.h"
#include "src/lib/darknet-adaptor/adaptors/orig-includes.h"

namespace darknet_adaptor::loaders
{

namespace
{

/// @brief The strip() of the original: removes every whitespace of the line.
std::string strip(const std::string& line)
{
  std::string stripped;
  stripped.reserve(line.size());

  for (const char symbol : line) {
    if (symbol != ' ' && symbol != '\t' && symbol != '\n' && symbol != '\r') {
      stripped.push_back(symbol);
    }
  }

  return stripped;
}

}  // namespace

CfgSection::CfgSection(std::string ntype) : mtype{std::move(ntype)} {}

CfgSections CfgSection::read_cfg(const std::string& filename)
{
  CLOGT("Trying to read the file: %s", filename.c_str());

  std::ifstream file{filename};

  if (!file.is_open()) {
    CLOGE("Failure to open options file: %s", filename.c_str());
    throw adaptors::NetworkError("Couldn't open the cfg file: " + filename);
  }

  return read_cfg(file, filename);
}

CfgSections CfgSection::read_cfg(std::istream& text, const std::string& name)
{
  CfgSections sections;
  std::string raw;
  int nu = 0;

  while (std::getline(text, raw)) {
    ++nu;

    const std::string line = strip(raw);

    CLOGT("Reading line: %s", line.c_str());

    if (line.empty() || line[0] == '#' || line[0] == ';') {
      continue;
    }

    if (line[0] == '[') {
      sections.emplace_back(line);
      continue;
    }

    if (sections.empty()) {
      CLOGEM("No cfg file section declared!");
      throw adaptors::NetworkError(
          "The cfg file option before the first section: " + name);
    }

    if (!sections.back().add(line)) {
      CLOGE("Config file error line %d, could parse: %s", nu,
            line.substr(0, line.find('=')).c_str());
    }
  }

  return sections;
}

const std::string& CfgSection::type() const { return mtype; }

bool CfgSection::add(const std::string& line)
{
  const size_t separator = line.find('=');

  if (separator == std::string::npos) {
    // the read_option() inserted the whole line as the key of the NULL value
    CLOGT("Inserting option %s = (null)", line.c_str());
    moptions.push_back(Option{line, std::string{}, false, false});
    return true;
  }

  if (separator + 1U == line.size()) {
    CLOGT("No option in line: %s", line.substr(0, separator).c_str());
    return false;
  }

  Option option{line.substr(0, separator), line.substr(separator + 1U), true,
                false};

  CLOGT("Inserting option %s = %s", option.key.c_str(), option.val.c_str());

  moptions.push_back(std::move(option));

  return true;
}

const char* CfgSection::find(const std::string& key)
{
  for (Option& option : moptions) {
    if (option.key == key) {
      option.used = true;
      return option.has_val ? option.val.c_str() : nullptr;
    }
  }

  return nullptr;
}

const char* CfgSection::find_str(const std::string& key, const char* def)
{
  const char* value = find(key);

  if (value) return value;

  if (def) CLOGI("%s: Using default '%s'", key.c_str(), def);

  return def;
}

const char* CfgSection::find_str_quiet(const std::string& key, const char* def)
{
  const char* value = find(key);

  if (value) return value;

  return def;
}

int CfgSection::find_int(const std::string& key, int def)
{
  const char* value = find(key);

  if (value) return std::atoi(value);

  CLOGI("%s: Using default '%d'", key.c_str(), def);

  return def;
}

int CfgSection::find_int_quiet(const std::string& key, int def)
{
  const char* value = find(key);

  if (value) return std::atoi(value);

  return def;
}

float CfgSection::find_float(const std::string& key, float def)
{
  const char* value = find(key);

  if (value) return static_cast<float>(std::atof(value));

  CLOGI("%s: Using default '%lf'", key.c_str(), static_cast<double>(def));

  return def;
}

float CfgSection::find_float_quiet(const std::string& key, float def)
{
  const char* value = find(key);

  if (value) return static_cast<float>(std::atof(value));

  return def;
}

void CfgSection::unused() const
{
  for (const Option& option : moptions) {
    if (!option.used) {
      CLOGW("Unused field: '%s = %s'", option.key.c_str(),
            option.has_val ? option.val.c_str() : "(null)");
    }
  }
}

std::vector<std::pair<std::string, const char*>> CfgSection::options() const
{
  std::vector<std::pair<std::string, const char*>> result;

  for (const Option& option : moptions) {
    result.emplace_back(option.key,
                        option.has_val ? option.val.c_str() : nullptr);
  }

  return result;
}

}  // namespace darknet_adaptor::loaders
