#ifndef DARKNET_CXX_PORT_PROJECT_DXXWJZ3_V3_NETWORK_BUILDER_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_DXXWJZ3_V3_NETWORK_BUILDER_CLASS_H

#include <vector>

#include "src/NetworksLoaders/dxxwjz/dxxwjz2/Dxxwjz2SubtreeBuilder.h"
#include "src/NetworksLoaders/dxxwjz/dxxwjz3/Dxxwjz3MetadataReader.h"
#include "src/NetworksLoaders/weights-model/WeightsDocument.h"

namespace darknet_adaptor::loaders::dxxwjz3::v3
{

/**
 * @brief Builds the tree of the network value of the dxxwjz3 document (see
 * the Dxxwjz3Format) the way the Dxxwjz2SubtreeBuilder does, of no arrays:
 * the arrays of a layer object go into the layers of the document once the
 * object is complete (see the Dxxwjz3MetadataReader::arrays()), so the tree
 * keeps the weights of a layer at a time.
 */
class Dxxwjz3NetworkBuilder : public dxxwjz2::v2::Dxxwjz2SubtreeBuilder
{
 public:
  virtual ~Dxxwjz3NetworkBuilder() = default;

  /// @param nreader The reader of the arrays, which outlives the builder.
  /// @param nlayers The layers of the arrays, which outlive the builder.
  Dxxwjz3NetworkBuilder(Failure nfail, Dxxwjz3MetadataReader& nreader,
                        std::vector<WeightsLayer>& nlayers);

 protected:
  bool closed(json& value) override;

 private:
  Failure mfail;
  Dxxwjz3MetadataReader& mreader;
  std::vector<WeightsLayer>& mlayers;
};

}  // namespace darknet_adaptor::loaders::dxxwjz3::v3

#endif  // DARKNET_CXX_PORT_PROJECT_DXXWJZ3_V3_NETWORK_BUILDER_CLASS_H
