#ifndef DARKNET_CXX_PORT_PROJECT_DXXWJZ3_V3_SAX_HANDLER_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_DXXWJZ3_V3_SAX_HANDLER_CLASS_H

#include <cstdint>
#include <string>

#include "src/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz2/v2/Dxxwjz2SaxHandler.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz3/v3/Dxxwjz3Bytes.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz3/v3/Dxxwjz3MetadataReader.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz3/v3/Dxxwjz3NetworkBuilder.h"

namespace darknet_adaptor::loaders::dxxwjz3::v3
{

/**
 * @brief Builds the document of the SAX events of the dxxwjz3 JSON text (see
 * the Dxxwjz3Format) the way the dxxwjz2 handler does: the cfg text and the
 * layers of the network member of the Dxxwjz3NetworkBuilder and the
 * Dxxwjz3MetadataReader, the trailing bytes of their array. The peek reads
 * the whole network, which keeps the weights.
 */
class Dxxwjz3SaxHandler : public dxxwjz2::v2::Dxxwjz2SaxHandler
{
 public:
  virtual ~Dxxwjz3SaxHandler() = default;
  Dxxwjz3SaxHandler(WeightsDocument& document, WeightsReport& report,
                    bool npeek = false);

 protected:
  const char* format_name() const override;
  int64_t format_version() const override;
  /// @brief The layers of the dxxwjz3 documents are the network ones.
  bool layers_member() const override;
  dxxwjz2::v2::Dxxwjz2MetadataReader& metadata() override;
  dxxwjz2::v2::Dxxwjz2SubtreeBuilder& builder(const std::string& name) override;
  Delegate* trailing() override;

 private:
  Dxxwjz3MetadataReader mdescription;
  Dxxwjz3NetworkBuilder mnetwork;
  Dxxwjz3Bytes mbytes;
};

}  // namespace darknet_adaptor::loaders::dxxwjz3::v3

#endif  // DARKNET_CXX_PORT_PROJECT_DXXWJZ3_V3_SAX_HANDLER_CLASS_H
