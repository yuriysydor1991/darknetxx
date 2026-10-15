#include "src/gui/net-editor/NetworkModel.h"

#include <algorithm>
#include <cctype>
#include <sstream>

#include "src/gui/net-editor/LayerReferences.h"

namespace net_editor
{

NetworkModel::NetworkModel() : mnet{std::make_shared<NetworkSection>("net")} {}

bool NetworkModel::read(const std::string& cfg)
{
  std::istringstream text{cfg};
  std::string line;
  std::vector<NetworkSectionPtr> sections;
  NetworkSection::Options options;

  const auto close = [&sections, &options]() {
    if (!sections.empty()) {
      sections.back() = std::make_shared<NetworkSection>(
          sections.back()->type(), std::move(options));
    }

    options.clear();
  };

  while (std::getline(text, line)) {
    line.erase(std::remove_if(line.begin(), line.end(),
                              [](unsigned char symbol) {
                                return std::isspace(symbol) != 0;
                              }),
               line.end());

    if (line.empty() || line[0] == '#' || line[0] == ';') {
      continue;
    }

    if (line[0] == '[') {
      close();
      sections.push_back(std::make_shared<NetworkSection>(line.substr(
          1, line.find(']') == std::string::npos ? std::string::npos
                                                 : line.find(']') - 1)));
      continue;
    }

    const std::size_t equal = line.find('=');

    options.emplace_back(line.substr(0, equal), equal == std::string::npos
                                                    ? std::string{}
                                                    : line.substr(equal + 1));
  }

  close();

  if (sections.empty() ||
      (sections[0]->type() != "net" && sections[0]->type() != "network")) {
    return false;
  }

  mnet = sections[0];
  mlayers.assign(sections.begin() + 1, sections.end());

  return true;
}

std::string NetworkModel::cfg() const
{
  std::string text = mnet->cfg();

  for (const auto& layer : mlayers) {
    text += "\n" + layer->cfg();
  }

  return text;
}

const NetworkSectionPtr& NetworkModel::net() const { return mnet; }

const std::vector<NetworkSectionPtr>& NetworkModel::layers() const
{
  return mlayers;
}

int NetworkModel::index_of(const NetworkSection& section) const
{
  const auto found = std::find_if(mlayers.begin(), mlayers.end(),
                                  [&section](const NetworkSectionPtr& layer) {
                                    return layer.get() == &section;
                                  });

  return found == mlayers.end() ? -1
                                : static_cast<int>(found - mlayers.begin());
}

const NetworkSection* NetworkModel::next(const int index) const
{
  const auto following = static_cast<std::size_t>(index + 1);

  return index + 1 < 0 || following >= mlayers.size()
             ? nullptr
             : mlayers[following].get();
}

NetworkSectionPtr NetworkModel::add(const std::string& type,
                                    NetworkSection::Options options)
{
  mlayers.push_back(std::make_shared<NetworkSection>(type, std::move(options)));

  return mlayers.back();
}

void NetworkModel::remove(const int index)
{
  std::vector<NetworkSectionPtr> nlayers = mlayers;

  nlayers.erase(nlayers.begin() + index);
  reorder(std::move(nlayers));
}

void NetworkModel::move(const int from, const int to)
{
  std::vector<NetworkSectionPtr> nlayers = mlayers;
  const NetworkSectionPtr moved = nlayers[static_cast<std::size_t>(from)];

  nlayers.erase(nlayers.begin() + from);
  nlayers.insert(nlayers.begin() + to, moved);
  reorder(std::move(nlayers));
}

void NetworkModel::reorder(std::vector<NetworkSectionPtr> nlayers)
{
  LayerReferences::remap(mlayers, nlayers);
  mlayers = std::move(nlayers);
}

}  // namespace net_editor
