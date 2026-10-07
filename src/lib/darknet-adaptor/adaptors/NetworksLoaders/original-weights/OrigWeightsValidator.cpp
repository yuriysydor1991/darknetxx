#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/original-weights/OrigWeightsValidator.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <string>
#include <system_error>

#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/original-weights/OrigWeightsReader.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/weights-model/WeightsLayout.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/weights-model/WeightsReport.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/weights-model/WeightsSchema.h"
#include "src/lib/darknet-adaptor/adaptors/orig-includes.h"

namespace darknet_adaptor::loaders
{

namespace
{

/// @brief How far the arrays of the document follow the arrays of a layout.
struct ArraysSequence
{
  /// the arrays of the document, which follow the layout ones
  std::size_t followed{0U};
  /// the reason the rest of the document arrays doesn't follow the layout
  /// ones, empty for the document which follows the layout
  std::string gap;
};

/**
 * @brief Follows the arrays of the document along the arrays of the layout:
 * the arrays of the layout from the first one on, the complete ones but the
 * last one of the document.
 */
ArraysSequence follow(const WeightsDocument& document,
                      const WeightsLayoutEntries& layout)
{
  ArraysSequence sequence;

  auto entry = layout.begin();
  // the array of the document shorter than the layout one
  std::string shorter;

  for (const WeightsLayer& l : document.layers) {
    for (const WeightsArray& a : l.arrays) {
      const std::string array = WeightsSchema::describe(l.index, a.sub, a.kind);

      if (!shorter.empty()) {
        sequence.gap = shorter + " and " + array + " follows it";
        return sequence;
      }

      const auto found = std::find_if(
          entry, layout.end(), [&l, &a](const WeightsLayoutEntry& e) {
            return e.layer == l.index && e.sub == a.sub && e.kind == a.kind;
          });

      if (found == layout.end()) {
        sequence.gap = array + " isn't an array of the network";
        return sequence;
      }

      if (found != entry) {
        sequence.gap =
            "the document misses " +
            WeightsSchema::describe(entry->layer, entry->sub, entry->kind) +
            " and holds " + array + " after it";
        return sequence;
      }

      if (a.values.size() > entry->count) {
        sequence.gap = array + " holds " + std::to_string(a.values.size()) +
                       " values, the network one takes " +
                       std::to_string(entry->count);
        return sequence;
      }

      if (a.values.size() < entry->count) {
        shorter = array + " holds " + std::to_string(a.values.size()) +
                  " of the " + std::to_string(entry->count) +
                  " values of the network one";
      }

      ++sequence.followed;
      ++entry;
    }
  }

  return sequence;
}

}  // namespace

uint64_t OrigWeightsInspection::expected_floats() const
{
  return loading_layout ? loaded_floats : saved_floats;
}

bool OrigWeightsValidator::inspect(const std::string& filename,
                                   const network& net,
                                   OrigWeightsInspection& inspection)
{
  std::error_code error;
  const uintmax_t size = std::filesystem::file_size(filename, error);

  std::ifstream file{filename, std::ios::binary};

  if (error || !file.is_open()) {
    CLOGE("Couldn't open file: %s", filename.c_str());
    return false;
  }

  WeightsHeader header;

  if (!OrigWeightsReader::read_header(file, header)) {
    CLOGE("The weights file %s is shorter than its header", filename.c_str());
    return false;
  }

  WeightsLayoutOptions loading;
  loading.loading = true;

  inspection = OrigWeightsInspection{};
  inspection.header = header;
  inspection.file_size = static_cast<uint64_t>(size);
  inspection.payload = inspection.file_size - header.size();
  inspection.saved_floats = WeightsLayout::floats(WeightsLayout::of(net));
  inspection.loaded_floats =
      WeightsLayout::floats(WeightsLayout::of(net, loading));
  inspection.loading_layout =
      inspection.payload != inspection.saved_floats * sizeof(float) &&
      inspection.payload == inspection.loaded_floats * sizeof(float);

  return true;
}

WeightsReport OrigWeightsValidator::validate(const std::string& filename,
                                             const network& net)
{
  OrigWeightsInspection inspection;

  if (!inspect(filename, net, inspection)) {
    WeightsReport report;
    report.error("the file can't be read or is shorter than the header");
    return report;
  }

  return validate(inspection);
}

WeightsReport OrigWeightsValidator::validate(
    const OrigWeightsInspection& inspection)
{
  WeightsReport report;

  // the gzip magic bytes are the major version of no darknet weights file,
  // while every dxxwjz1 file starts with them
  static constexpr const uint32_t GZIP_MAGIC = 0x8B1FU;

  if ((static_cast<uint32_t>(inspection.header.major) & 0xFFFFU) ==
      GZIP_MAGIC) {
    report.error(
        "the file starts with the gzip magic bytes: it's the gzip compressed "
        "file (the dxxwjz1 one?) rather than the original weights one");
    return report;
  }

  const uint64_t expected = inspection.expected_floats() * sizeof(float);

  if (inspection.loading_layout) {
    report.warning(
        "the file holds the arrays the network loads: no arrays of the "
        "dontload layers and no batch normalization arrays of the "
        "dontloadscales ones");
  }

  if (inspection.payload < expected) {
    report.warning("the file holds " + std::to_string(inspection.payload) +
                   " of the " + std::to_string(expected) +
                   " bytes of the network arrays, the rest of the arrays "
                   "keeps the network values (the truncated file, the file "
                   "saved up to a layer or the file of another network)");
  } else if (inspection.payload > expected) {
    report.warning("the file holds " +
                   std::to_string(inspection.payload - expected) +
                   " bytes after the network arrays, which the loaders "
                   "ignore (the file of another network?)");
  }

  if (inspection.payload < expected &&
      inspection.payload % sizeof(float) != 0U) {
    report.warning("the file ends in the " +
                   std::to_string(inspection.payload % sizeof(float)) +
                   " bytes of a float, which the loaders ignore");
  }

  return report;
}

WeightsReport OrigWeightsValidator::validate(const WeightsDocument& document,
                                             const network& net)
{
  WeightsReport report;

  WeightsLayoutOptions loading;
  loading.loading = true;

  // the file of the arrays the save_weights() writes or the one of the
  // arrays the load_weights() reads (the file made for the network of the
  // dontload and the dontloadscales layers)
  const ArraysSequence saved = follow(document, WeightsLayout::of(net));

  if (saved.gap.empty()) {
    return report;
  }

  const ArraysSequence loaded =
      follow(document, WeightsLayout::of(net, loading));

  if (loaded.gap.empty()) {
    return report;
  }

  report.error(
      "the document can't be written as the original weights file of the "
      "network, which keeps the arrays one after another with no names of "
      "them: " +
      (loaded.followed > saved.followed ? loaded.gap : saved.gap));

  return report;
}

}  // namespace darknet_adaptor::loaders
