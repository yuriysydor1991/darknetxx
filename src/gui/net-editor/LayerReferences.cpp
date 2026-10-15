#include "src/gui/net-editor/LayerReferences.h"

#include <algorithm>
#include <cstdlib>

namespace net_editor
{

namespace
{

/// @brief The index of the section among the layers, -1 for none.
int index_in(const std::vector<NetworkSectionPtr>& layers,
             const NetworkSectionPtr& section)
{
  const auto found = std::find(layers.begin(), layers.end(), section);

  return found == layers.end() ? -1 : static_cast<int>(found - layers.begin());
}

}  // namespace

std::string LayerReferences::key_of(const std::string& type)
{
  if (type == "route") {
    return "layers";
  }

  return type == "shortcut" || type == "scale_channels" || type == "sam"
             ? "from"
             : std::string{};
}

void LayerReferences::remap(const std::vector<NetworkSectionPtr>& before,
                            const std::vector<NetworkSectionPtr>& after)
{
  for (std::size_t self = 0U; self < after.size(); ++self) {
    const NetworkSectionPtr& layer = after[self];
    const std::string key = key_of(layer->type());
    const std::string* value = key.empty() ? nullptr : layer->find(key);
    const int old = index_in(before, layer);

    if (value == nullptr || old < 0) {
      continue;
    }

    std::string remapped;
    const char* separator = "";

    for (const std::string& piece : NetworkSection::split(*value)) {
      char* end = nullptr;
      const long reference = std::strtol(piece.c_str(), &end, 10);
      const long target = reference < 0 ? old + reference : reference;
      const int moved =
          piece.empty() || *end != '\0' || target < 0 ||
                  target >= static_cast<long>(before.size())
              ? -1
              : index_in(after, before[static_cast<std::size_t>(target)]);
      const int relative = moved - static_cast<int>(self);
      std::string text = piece;

      // a reference turned forward is the absolute one the validation tells
      if (moved >= 0) {
        text = std::to_string(reference < 0 && relative < 0 ? relative : moved);
      }

      remapped += separator + text;
      separator = ",";
    }

    if (remapped != *value) {
      layer->set(key, remapped);
    }
  }
}

}  // namespace net_editor
