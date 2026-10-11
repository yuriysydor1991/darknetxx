#ifndef DARKNET_CXX_PORT_PROJECT_DXXWJZ3P_V1_SAX_HANDLER_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_DXXWJZ3P_V1_SAX_HANDLER_CLASS_H

#include <cstdint>
#include <filesystem>
#include <string>

#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz2p/v1/Dxxwjz2pSaxHandler.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz3/v3/Dxxwjz3Bytes.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz3/v3/Dxxwjz3MetadataReader.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz3/v3/Dxxwjz3NetworkBuilder.h"

namespace darknet_adaptor::loaders::dxxwjz3p::v1
{

/**
 * @brief Builds the document of the SAX events of the dxxwjz3p JSON text (see
 * the Dxxwjz3pFormat) the way the dxxwjz2p handler does, the network member
 * and the trailing bytes the way the dxxwjz3 one does (see the
 * Dxxwjz3SaxHandler).
 */
class Dxxwjz3pSaxHandler : public dxxwjz2p::v1::Dxxwjz2pSaxHandler
{
 public:
  virtual ~Dxxwjz3pSaxHandler() = default;

  /// @param directory The directory of the file, of the relative db paths.
  Dxxwjz3pSaxHandler(WeightsDocument& document, WeightsReport& report,
                     std::filesystem::path directory, bool npeek = false);

 protected:
  const char* format_name() const override;
  int64_t format_version() const override;
  /// @brief The layers of the dxxwjz3 documents are the network ones.
  bool layers_member() const override;
  dxxwjz2::v2::Dxxwjz2MetadataReader& metadata() override;
  dxxwjz2::v2::Dxxwjz2SubtreeBuilder& builder(const std::string& name) override;
  Delegate* trailing() override;

 private:
  dxxwjz3::v3::Dxxwjz3MetadataReader mdescription;
  dxxwjz3::v3::Dxxwjz3NetworkBuilder mnetwork;
  dxxwjz3::v3::Dxxwjz3Bytes mbytes;
};

}  // namespace darknet_adaptor::loaders::dxxwjz3p::v1

#endif  // DARKNET_CXX_PORT_PROJECT_DXXWJZ3P_V1_SAX_HANDLER_CLASS_H
