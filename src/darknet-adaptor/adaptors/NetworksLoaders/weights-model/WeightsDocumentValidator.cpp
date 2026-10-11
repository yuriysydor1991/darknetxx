#include "src/darknet-adaptor/adaptors/NetworksLoaders/weights-model/WeightsDocumentValidator.h"

#include <cstdint>
#include <limits>
#include <map>
#include <string>
#include <tuple>

#include "src/darknet-adaptor/adaptors/NetworksLoaders/weights-model/WeightsDocument.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/weights-model/WeightsLayout.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/weights-model/WeightsReport.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/weights-model/WeightsSchema.h"

namespace darknet_adaptor::loaders
{

namespace
{

using ArrayKey = std::tuple<int, WeightsSubLayer, WeightsArrayKind>;

std::string layer_of(const WeightsLayer& l)
{
  return "the layer " + std::to_string(l.index) + " (" +
         WeightsSchema::name(l.type) + ")";
}

void validate_arrays(const WeightsLayer& l, WeightsReport& report)
{
  int previous = -1;

  for (const WeightsArray& a : l.arrays) {
    const int rank = WeightsSchema::rank(l.type, a.sub, a.kind);
    const std::string array = WeightsSchema::describe(l.index, a.sub, a.kind);

    if (rank < 0) {
      report.error(WeightsSchema::has_weights(l.type)
                       ? layer_of(l) + " has no " +
                             (a.sub == WeightsSubLayer::None
                                  ? std::string{}
                                  : WeightsSchema::name(a.sub) + " ") +
                             WeightsSchema::name(a.kind) + " array"
                       : layer_of(l) + " keeps no weights, but " + array +
                             " is given");
      continue;
    }

    if (rank == previous) {
      report.error(array + " is given twice");
    } else if (rank < previous) {
      report.error(array + " is out of the order of the " +
                   WeightsSchema::name(l.type) + " layer arrays");
    }

    previous = rank;
  }
}

}  // namespace

WeightsReport WeightsDocumentValidator::validate(
    const WeightsDocument& document)
{
  WeightsReport report;

  if (!document.header.seen64() &&
      document.header.seen > std::numeric_limits<uint32_t>::max()) {
    report.error("the header of the " + document.header.version() +
                 " version keeps the 32 bits seen, which can't hold the " +
                 std::to_string(document.header.seen) + " one");
  }

  int previous = -1;

  for (const WeightsLayer& l : document.layers) {
    if (l.index < 0) {
      report.error("the layer index " + std::to_string(l.index) +
                   " is negative");
    } else if (l.index == previous) {
      report.error("the layer " + std::to_string(l.index) + " is given twice");
    } else if (l.index < previous) {
      report.error("the layer " + std::to_string(l.index) +
                   " follows the layer " + std::to_string(previous) +
                   ", the layers are out of the order");
    }

    previous = l.index;

    validate_arrays(l, report);
  }

  return report;
}

WeightsReport WeightsDocumentValidator::validate(
    const WeightsDocument& document, const network& net)
{
  WeightsReport report = validate(document);

  // the arrays the network keeps and the ones its loaders read
  std::map<ArrayKey, std::size_t> kept;

  for (const WeightsLayoutEntry& entry : WeightsLayout::of(net)) {
    kept.emplace(ArrayKey{entry.layer, entry.sub, entry.kind}, entry.count);
  }

  for (const WeightsLayer& l : document.layers) {
    if (l.index < 0) {
      continue;
    }

    if (l.index >= net.n) {
      report.error(layer_of(l) + " is beyond the " + std::to_string(net.n) +
                   " layers of the network");
      continue;
    }

    const LAYER_TYPE type = net.layers[l.index].type;

    if (type != l.type) {
      report.error(layer_of(l) + " is the " + WeightsSchema::name(type) +
                   " layer in the network");
      continue;
    }

    for (const WeightsArray& a : l.arrays) {
      const std::string array = WeightsSchema::describe(l.index, a.sub, a.kind);
      const auto found = kept.find(ArrayKey{l.index, a.sub, a.kind});

      if (found == kept.end()) {
        if (WeightsSchema::rank(l.type, a.sub, a.kind) >= 0) {
          report.error(array + " isn't an array of the network (the " +
                       "network layer keeps no such weights)");
        }

        continue;
      }

      if (a.values.size() > found->second) {
        report.error(array + " holds " + std::to_string(a.values.size()) +
                     " values, the network one takes " +
                     std::to_string(found->second));
      } else if (a.values.size() < found->second) {
        report.warning(array + " holds " + std::to_string(a.values.size()) +
                       " of the " + std::to_string(found->second) +
                       " values of the network one, the rest of them keeps "
                       "the network values");
      }
    }
  }

  // the network arrays the loaders would read, which the document doesn't
  // have, reported once
  WeightsLayoutOptions loading;
  loading.loading = true;

  std::size_t missing = 0U;
  std::string first_missing;

  for (const WeightsLayoutEntry& entry : WeightsLayout::of(net, loading)) {
    const WeightsLayer* l = document.find(entry.layer);

    if (l != nullptr && l->type == entry.type &&
        l->find(entry.sub, entry.kind) != nullptr) {
      continue;
    }

    if (missing++ == 0U) {
      first_missing =
          WeightsSchema::describe(entry.layer, entry.sub, entry.kind);
    }
  }

  if (missing > 0U) {
    report.warning(std::to_string(missing) +
                   " arrays of the network aren't in the document (" +
                   first_missing +
                   " is the first of them), the network keeps their values");
  }

  return report;
}

}  // namespace darknet_adaptor::loaders
