#include "src/app/converter/ConversionFormats.h"

#include <string>
#include <utility>
#include <vector>

namespace app::converter
{

namespace
{

struct Spelling
{
  ConversionFormat format;
  std::string name;
  std::string description;
};

const std::vector<Spelling>& spellings()
{
  static const std::vector<Spelling> all{
      {ConversionFormat::weights, "weights",
       "the original darknet weights file (*.weights), which requires the "
       "network cfg file"},
      {ConversionFormat::dxxwjz1, "dxxwjz1",
       "the darknetxx weights JSON zipped file of the first version "
       "(*.dxxwjz1)"}};

  return all;
}

}  // namespace

bool ConversionFormats::parse(const std::string& name, ConversionFormat& format)
{
  for (const Spelling& spelling : spellings()) {
    if (spelling.name == name) {
      format = spelling.format;
      return true;
    }
  }

  return false;
}

const std::vector<std::string>& ConversionFormats::names()
{
  static const std::vector<std::string> all = []() {
    std::vector<std::string> result;

    for (const Spelling& spelling : spellings()) {
      result.push_back(spelling.name);
    }

    return result;
  }();

  return all;
}

const std::string& ConversionFormats::description(const std::string& name)
{
  for (const Spelling& spelling : spellings()) {
    if (spelling.name == name) {
      return spelling.description;
    }
  }

  static const std::string none;

  return none;
}

}  // namespace app::converter
