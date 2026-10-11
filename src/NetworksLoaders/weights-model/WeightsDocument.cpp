#include "src/NetworksLoaders/weights-model/WeightsDocument.h"

#include <algorithm>
#include <cstdint>
#include <string>
#include <vector>

#include "src/NetworksLoaders/weights-model/WeightsSchema.h"
#include "version.h"

namespace darknet_adaptor::loaders
{

WeightsHeader WeightsHeader::current(const uint64_t seen)
{
  WeightsHeader header;
  header.major = MAJOR_VERSION;
  header.minor = MINOR_VERSION;
  header.revision = PATCH_VERSION;
  header.seen = seen;
  return header;
}

bool WeightsHeader::seen64() const
{
  // computed in the 64 bits, which the original int one overflowed for the
  // huge major versions of the garbage files
  return static_cast<int64_t>(major) * 10 + static_cast<int64_t>(minor) >= 2;
}

bool WeightsHeader::transposed() const { return major > 1000 || minor > 1000; }

std::size_t WeightsHeader::size() const
{
  return 3U * sizeof(int32_t) +
         (seen64() ? sizeof(uint64_t) : sizeof(uint32_t));
}

std::string WeightsHeader::version() const
{
  return std::to_string(major) + "." + std::to_string(minor) + "." +
         std::to_string(revision);
}

bool WeightsHeader::operator==(const WeightsHeader& other) const
{
  return major == other.major && minor == other.minor &&
         revision == other.revision && seen == other.seen;
}

bool WeightsHeader::operator!=(const WeightsHeader& other) const
{
  return !(*this == other);
}

const WeightsArray* WeightsLayer::find(const WeightsSubLayer sub,
                                       const WeightsArrayKind kind) const
{
  const auto found = std::find_if(
      arrays.begin(), arrays.end(),
      [sub, kind](const auto& a) { return a.sub == sub && a.kind == kind; });

  return found == arrays.end() ? nullptr : &(*found);
}

const WeightsLayer* WeightsDocument::find(const int index) const
{
  const auto found = std::lower_bound(
      layers.begin(), layers.end(), index,
      [](const WeightsLayer& l, const int value) { return l.index < value; });

  return found == layers.end() || found->index != index ? nullptr : &(*found);
}

uint64_t WeightsDocument::floats() const
{
  uint64_t count = 0U;

  for (const WeightsLayer& l : layers) {
    for (const WeightsArray& a : l.arrays) {
      count += a.values.size();
    }
  }

  return count;
}

bool WeightsDocument::network_alone() const
{
  return layers.empty() && trailing.empty();
}

uint64_t WeightsDocument::file_size() const
{
  return header.size() + floats() * sizeof(float) + trailing.size();
}

void WeightsDocument::normalize()
{
  std::stable_sort(layers.begin(), layers.end(),
                   [](const WeightsLayer& a, const WeightsLayer& b) {
                     return a.index < b.index;
                   });

  for (WeightsLayer& l : layers) {
    const LAYER_TYPE type = l.type;

    std::stable_sort(l.arrays.begin(), l.arrays.end(),
                     [type](const WeightsArray& a, const WeightsArray& b) {
                       return WeightsSchema::rank(type, a.sub, a.kind) <
                              WeightsSchema::rank(type, b.sub, b.kind);
                     });
  }
}

}  // namespace darknet_adaptor::loaders
