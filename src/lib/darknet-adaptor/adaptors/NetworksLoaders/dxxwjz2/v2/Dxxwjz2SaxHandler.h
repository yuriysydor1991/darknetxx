#ifndef DARKNET_CXX_PORT_PROJECT_DXXWJZ2_V2_SAX_HANDLER_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_DXXWJZ2_V2_SAX_HANDLER_CLASS_H

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz1/v1/Dxxwjz1SaxHandler.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz2/v2/Dxxwjz2MetadataReader.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz2/v2/Dxxwjz2SubtreeBuilder.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/weights-model/WeightsDocument.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/weights-model/WeightsReport.h"

namespace darknet_adaptor::loaders::dxxwjz2::v2
{

/**
 * @brief Builds the document of the SAX events of the dxxwjz2 JSON text (see
 * the Dxxwjz2Format): the dxxwjz1 members by the dxxwjz1 handler, the network
 * and the training ones of their trees.
 *
 * The peek of the file stops at its first weight once the network is read,
 * so the description of the file is read without its weights.
 */
class Dxxwjz2SaxHandler : public dxxwjz1::v1::Dxxwjz1SaxHandler
{
 public:
  virtual ~Dxxwjz2SaxHandler() = default;
  Dxxwjz2SaxHandler(WeightsDocument& document, WeightsReport& report,
                    bool npeek = false);

  bool complete(bool parsed) const override;

 protected:
  const char* format_name() const override;
  int64_t format_version() const override;
  std::vector<std::string> required_keys() const override;
  Delegate* extension(const std::string& name) override;
  bool extension_end(const std::string& name) override;
  bool appended() override;

  /// @brief The reader of the network and the training trees.
  virtual Dxxwjz2MetadataReader& metadata();

  /// @brief The builder of the tree of the network or the training value:
  /// the subtree builder of both here.
  virtual Dxxwjz2SubtreeBuilder& builder(const std::string& name);

  /// @brief The handler of the trailing value, which builds the trailing bytes
  /// of the document itself; nullptr for the base64 text of the dxxwjz1
  /// handler here.
  virtual Delegate* trailing();

 private:
  Dxxwjz2SubtreeBuilder mbuilder;
  Dxxwjz2MetadataReader mmetadata;
  bool mpeek;
  bool mnetwork{false};
  bool mstopped{false};
};

}  // namespace darknet_adaptor::loaders::dxxwjz2::v2

#endif  // DARKNET_CXX_PORT_PROJECT_DXXWJZ2_V2_SAX_HANDLER_CLASS_H
