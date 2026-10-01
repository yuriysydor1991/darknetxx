#include "src/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz1/v1/Dxxwjz1Writer.h"

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "src/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz1/v1/Dxxwjz1JsonEmitter.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/weights-model/WeightsDocument.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/weights-model/WeightsDocumentValidator.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/weights-model/WeightsLayout.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/weights-model/WeightsReport.h"
#include "src/darknet-adaptor/adaptors/orig-includes.h"
#include "src/zlib/ZlibController.h"

namespace darknet_adaptor::loaders::dxxwjz1::v1
{

Dxxwjz1Writer::Dxxwjz1Writer(const int level) : mlevel{level} {}

Dxxwjz1Writer::Dxxwjz1WriterPtr Dxxwjz1Writer::create()
{
  return std::make_shared<Dxxwjz1Writer>();
}

bool Dxxwjz1Writer::save(network& net, const std::string& filename, int cutoff,
                         bool save_ema)
{
  CLOGI("Saving weights to %s", filename.c_str());

  std::unique_ptr<zlibi::GzipOStream> file =
      zlibi::ZlibController::create()->create_file_writer(filename, mlevel);

  if (file == nullptr) {
    CLOGE("Couldn't open file: %s", filename.c_str());
    return false;
  }

  // the seen of the current iteration, the way the save_weights() writes it
  (*net.seen) = static_cast<uint64_t>(get_current_iteration(net) * net.batch *
                                      net.subdivisions);

  WeightsLayoutOptions options;
  options.cutoff = cutoff;
  options.ema = save_ema;

  const WeightsLayoutEntries layout = WeightsLayout::of(net, options);

  Dxxwjz1JsonEmitter emitter{*file};

  emitter.begin(WeightsHeader::current(*net.seen));

  auto entry = layout.begin();
  std::vector<Dxxwjz1ArrayView> arrays;

  for (int i = 0; i < net.n && i < cutoff; ++i) {
    arrays.clear();

    for (; entry != layout.end() && entry->layer == i; ++entry) {
      arrays.push_back(
          Dxxwjz1ArrayView{entry->sub, entry->kind, entry->data, entry->count});
    }

    emitter.layer(i, net.layers[i].type, arrays);
  }

  emitter.end({});

  if (!file->close()) {
    CLOGE("Failure while writing the weights file: %s", filename.c_str());
    return false;
  }

  return true;
}

bool Dxxwjz1Writer::write(const WeightsDocument& document,
                          const std::string& filename)
{
  const WeightsReport report = WeightsDocumentValidator::validate(document);

  if (!report.ok()) {
    report.log(filename);
    CLOGE("The weights document of the errors isn't written: %s",
          filename.c_str());
    return false;
  }

  std::unique_ptr<zlibi::GzipOStream> file =
      zlibi::ZlibController::create()->create_file_writer(filename, mlevel);

  if (file == nullptr) {
    CLOGE("Couldn't open file: %s", filename.c_str());
    return false;
  }

  Dxxwjz1JsonEmitter emitter{*file};

  emitter.begin(document.header);

  std::vector<Dxxwjz1ArrayView> arrays;

  for (const WeightsLayer& l : document.layers) {
    arrays.clear();

    for (const WeightsArray& a : l.arrays) {
      arrays.push_back(
          Dxxwjz1ArrayView{a.sub, a.kind, a.values.data(), a.values.size()});
    }

    emitter.layer(l.index, l.type, arrays);
  }

  emitter.end(document.trailing);

  if (!file->close()) {
    CLOGE("Failure while writing the weights file: %s", filename.c_str());
    return false;
  }

  return true;
}

}  // namespace darknet_adaptor::loaders::dxxwjz1::v1
