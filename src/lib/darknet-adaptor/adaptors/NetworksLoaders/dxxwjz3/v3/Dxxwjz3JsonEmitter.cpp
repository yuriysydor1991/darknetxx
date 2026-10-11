#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz3/v3/Dxxwjz3JsonEmitter.h"

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <ostream>
#include <string>
#include <vector>

#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz1/v1/Dxxwjz1Format.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz1/v1/Dxxwjz1JsonString.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz2/v2/Dxxwjz2Format.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz3/v3/Dxxwjz3Bytes.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz3/v3/Dxxwjz3Format.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz3/v3/Dxxwjz3Losses.h"

namespace darknet_adaptor::loaders::dxxwjz3::v3
{

using dxxwjz1::v1::Dxxwjz1ArrayView;
using dxxwjz1::v1::Dxxwjz1Format;
using dxxwjz1::v1::Dxxwjz1JsonString;
using dxxwjz2::v2::Dxxwjz2Format;

Dxxwjz3JsonEmitter::Dxxwjz3JsonEmitter(std::ostream& out,
                                       const NetworkDescription& network,
                                       const TrainingRecord& training)
    : Dxxwjz2JsonEmitter{out, network, training},
      mcfg{network.cfg()},
      mclasses{network.classes()}
{
}

void Dxxwjz3JsonEmitter::begin(const WeightsHeader& header, const bool weights)
{
  mheader = header;
  mweights = weights;
  mlayers = 0U;

  // the header and the trailing bytes follow the network, see the end()
  Dxxwjz2JsonEmitter::begin(header, false);
}

void Dxxwjz3JsonEmitter::layer(const int index, const LAYER_TYPE,
                               const std::vector<Dxxwjz1ArrayView>& arrays)
{
  assert(index >= 0 && static_cast<std::size_t>(index) < mcfg.layers().size());

  layers_before(static_cast<std::size_t>(index));

  write((mlayers++ == 0U ? "\n    {" : ",\n    {") + mcfg.layers()[index]);
  this->arrays(arrays);
}

void Dxxwjz3JsonEmitter::end(const std::vector<uint8_t>& bytes)
{
  layers_before(mcfg.layers().size());

  write(mlayers == 0U ? "], " : "\n  ], ");
  class_names();
  write("}");
  members();

  if (mweights) {
    write(",\n  " + header_member(mheader));
  }

  if (mweights && !bytes.empty()) {
    write(",\n  " + key(Dxxwjz1Format::TRAILING_KEY) + trailing(bytes));
  }

  // the end of the document of no layers member
  Dxxwjz2JsonEmitter::end({});
}

const char* Dxxwjz3JsonEmitter::format_name() const
{
  return Dxxwjz3Format::NAME;
}

int64_t Dxxwjz3JsonEmitter::format_version() const
{
  return Dxxwjz3Format::VERSION;
}

void Dxxwjz3JsonEmitter::sections()
{
  write(",\n  " + key(Dxxwjz2Format::NETWORK_KEY) + "{" +
        key(Dxxwjz3Format::NET_KEY) + "{" + mcfg.net() + "}, " +
        key(Dxxwjz1Format::LAYERS_KEY) + "[");
}

std::string Dxxwjz3JsonEmitter::trailing(
    const std::vector<uint8_t>& bytes) const
{
  return Dxxwjz3Bytes::json_of(bytes);
}

void Dxxwjz3JsonEmitter::losses(const TrainingRecord& training)
{
  columns(Dxxwjz3Losses::indexed(training.iterations(), training.losses()),
          Dxxwjz3Losses::indexed(training.iterations(), training.avg_losses()));
}

void Dxxwjz3JsonEmitter::members() { training(); }

void Dxxwjz3JsonEmitter::layers_before(const std::size_t index)
{
  while (mlayers < index) {
    layer(static_cast<int>(mlayers), BLANK, {});
  }
}

void Dxxwjz3JsonEmitter::class_names()
{
  std::string names;

  for (const std::string& name : mclasses) {
    names += (names.empty() ? "" : ", ") + Dxxwjz1JsonString::quote(name);
  }

  write(key(Dxxwjz2Format::CLASSES_KEY) + "[" + names + "]");
}

}  // namespace darknet_adaptor::loaders::dxxwjz3::v3
