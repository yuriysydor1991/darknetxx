#ifndef DARKNET_CXX_PORT_PROJECT_DXXWJZ2P_V1_SAX_HANDLER_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_DXXWJZ2P_V1_SAX_HANDLER_CLASS_H

#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

#include "src/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz2/v2/Dxxwjz2SaxHandler.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz2p/v1/Dxxwjz2pAnnotationsReader.h"

namespace darknet_adaptor::loaders::dxxwjz2p::v1
{

/**
 * @brief Builds the document of the SAX events of the dxxwjz2p JSON text (see
 * the Dxxwjz2pFormat): the dxxwjz2 members by the dxxwjz2 handler, the
 * annotations of their tree once the classes of the network are read, which
 * the writers write before them.
 *
 * The peek of the file stops at its first weight once both the network and
 * the annotations are read.
 */
class Dxxwjz2pSaxHandler : public dxxwjz2::v2::Dxxwjz2SaxHandler
{
 public:
  virtual ~Dxxwjz2pSaxHandler() = default;

  /// @param directory The directory of the file, of the relative db paths.
  Dxxwjz2pSaxHandler(WeightsDocument& document, WeightsReport& report,
                     std::filesystem::path directory, bool npeek = false);

 protected:
  const char* format_name() const override;
  int64_t format_version() const override;
  std::vector<std::string> required_keys() const override;
  Delegate* extension(const std::string& name) override;
  bool extension_end(const std::string& name) override;
  bool appended() override;

 private:
  /// @brief Reads the annotations of the tree once the network is read.
  bool annotate();

  dxxwjz2::v2::Dxxwjz2SubtreeBuilder mbuilder;
  Dxxwjz2pAnnotationsReader mreader;
  bool mnetwork{false};
  bool mannotated{false};
  bool mpending{false};
};

}  // namespace darknet_adaptor::loaders::dxxwjz2p::v1

#endif  // DARKNET_CXX_PORT_PROJECT_DXXWJZ2P_V1_SAX_HANDLER_CLASS_H
