#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz1/v1/Dxxwjz1Reader.h"

#include <algorithm>
#include <cstring>
#include <memory>
#include <set>
#include <string>

#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz1/v1/Dxxwjz1Validator.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/weights-model/WeightsDocument.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/weights-model/WeightsLayout.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/weights-model/WeightsReport.h"
#include "src/lib/darknet-adaptor/adaptors/orig-includes.h"

namespace darknet_adaptor::loaders::dxxwjz1::v1
{

Dxxwjz1Reader::Dxxwjz1ReaderPtr Dxxwjz1Reader::create()
{
  return std::make_shared<Dxxwjz1Reader>();
}

bool Dxxwjz1Reader::load(network& net, const std::string& filename, int cutoff)
{
  WeightsDocument rest;

  return load(net, filename, cutoff, rest);
}

bool Dxxwjz1Reader::load(network& net, const std::string& filename,
                         const int cutoff, WeightsDocument& rest)
{
  CLOGI("Loading weights from %s...", filename.c_str());

  if (!read(filename, &net, rest)) {
    return false;
  }

  const int loaded = network_to_train(rest) ? 0 : apply(rest, net, cutoff);

  rest.layers.clear();
  rest.layers.shrink_to_fit();
  rest.trailing.clear();

  CLOGI(" trained: %.0f K-images (%.0f Kilo-batches_64)",
        static_cast<double>(static_cast<float>(*net.seen / 1000)),
        static_cast<double>(static_cast<float>(*net.seen / 64000)));
  CLOGI("Done! Loaded %d layers from the file %s", loaded, filename.c_str());

  return true;
}

bool Dxxwjz1Reader::read(const std::string& filename, const network* net,
                         WeightsDocument& document)
{
  const WeightsReport report =
      Dxxwjz1Validator::validate(filename, net, document);

  report.log(filename);

  if (!report.ok()) {
    CLOGE("Couldn't read the dxxwjz1 file: %s", filename.c_str());
    return false;
  }

  return true;
}

bool Dxxwjz1Reader::network_to_train(WeightsDocument&) const { return false; }

int Dxxwjz1Reader::apply(const WeightsDocument& document, network& net,
                         const int cutoff)
{
  *net.seen = document.header.seen;
  *net.cur_iteration = get_current_batch(net);

  WeightsLayoutOptions options;
  options.cutoff = cutoff;
  options.loading = true;
  options.transposed = document.header.transposed();

  std::set<int> loaded;

  for (const WeightsLayoutEntry& entry : WeightsLayout::of(net, options)) {
    const WeightsLayer* l = document.find(entry.layer);
    const WeightsArray* a =
        l == nullptr ? nullptr : l->find(entry.sub, entry.kind);

    if (a == nullptr || entry.data == nullptr) {
      continue;
    }

    // the validated array is the network one or its beginning
    const std::size_t count = std::min(a->values.size(), entry.count);

    if (count > 0U) {
      std::memcpy(entry.data, a->values.data(), count * sizeof(float));
    }

    if (entry.rows > 0 && entry.cols > 0) {
      WeightsLayout::transpose_matrix(entry.data, entry.rows, entry.cols);
    }

    loaded.insert(entry.layer);
  }

  return static_cast<int>(loaded.size());
}

}  // namespace darknet_adaptor::loaders::dxxwjz1::v1
