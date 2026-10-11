#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz3/v3/Dxxwjz3NetworkBuilder.h"

#include <vector>

#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz1/v1/Dxxwjz1Format.h"

namespace darknet_adaptor::loaders::dxxwjz3::v3
{

Dxxwjz3NetworkBuilder::Dxxwjz3NetworkBuilder(Failure nfail,
                                             Dxxwjz3MetadataReader& nreader,
                                             std::vector<WeightsLayer>& nlayers)
    : Dxxwjz2SubtreeBuilder{nfail},
      mfail{nfail},
      mreader{nreader},
      mlayers{nlayers}
{
}

bool Dxxwjz3NetworkBuilder::closed(json& value)
{
  const auto layers = tree().find(dxxwjz1::v1::Dxxwjz1Format::LAYERS_KEY);

  // the values of the layers array of the network alone
  if (layers == tree().end() || !layers->is_array() || layers->empty() ||
      &layers->back() != &value) {
    return true;
  }

  return mreader.arrays(value, static_cast<int>(layers->size() - 1U),
                        mlayers) ||
         mfail(mreader.error());
}

}  // namespace darknet_adaptor::loaders::dxxwjz3::v3
