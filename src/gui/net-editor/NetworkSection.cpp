#include "src/gui/net-editor/NetworkSection.h"

#include <algorithm>
#include <cstdlib>
#include <sstream>

namespace net_editor
{

NetworkSection::NetworkSection(std::string ntype, Options noptions)
    : mtype{std::move(ntype)}, moptions{std::move(noptions)}
{
}

const std::string& NetworkSection::type() const { return mtype; }

const NetworkSection::Options& NetworkSection::options() const
{
  return moptions;
}

const std::string* NetworkSection::find(const std::string& key) const
{
  const auto option =
      std::find_if(moptions.begin(), moptions.end(),
                   [&key](const Option& one) { return one.first == key; });

  return option == moptions.end() ? nullptr : &option->second;
}

void NetworkSection::set(const std::string& key, const std::string& value)
{
  const auto option =
      std::find_if(moptions.begin(), moptions.end(),
                   [&key](const Option& one) { return one.first == key; });

  if (value.empty()) {
    if (option != moptions.end()) {
      moptions.erase(option);
    }
  } else if (option == moptions.end()) {
    moptions.emplace_back(key, value);
  } else {
    option->second = value;
  }
}

int NetworkSection::integer(const std::string& key, const int def) const
{
  const std::string* value = find(key);

  return value == nullptr || value->empty() ? def : std::atoi(value->c_str());
}

std::vector<int> NetworkSection::integers(const std::string& key) const
{
  const std::string* value = find(key);
  std::vector<int> result;

  for (const std::string& piece : split(value == nullptr ? "" : *value)) {
    result.push_back(std::atoi(piece.c_str()));
  }

  return result;
}

std::size_t NetworkSection::count(const std::string& key) const
{
  const std::string* value = find(key);

  return value == nullptr ? 0U : split(*value).size();
}

std::string NetworkSection::cfg() const
{
  std::string text = "[" + mtype + "]\n";

  for (const auto& [key, value] : moptions) {
    text += value.empty() ? key + "\n" : key + "=" + value + "\n";
  }

  return text;
}

std::string NetworkSection::summary() const
{
  std::string text;

  for (const auto& [key, value] : moptions) {
    text +=
        (text.empty() ? "" : " ") + key + (value.empty() ? "" : "=" + value);
  }

  return text;
}

std::vector<std::string> NetworkSection::split(const std::string& text)
{
  std::vector<std::string> pieces;
  std::istringstream stream{text};
  std::string piece;

  while (std::getline(stream, piece, ',')) {
    pieces.push_back(piece);
  }

  // the trailing comma ends an empty value the way the darknet reads it
  if (!text.empty() && text.back() == ',') {
    pieces.emplace_back();
  }

  return pieces;
}

}  // namespace net_editor
