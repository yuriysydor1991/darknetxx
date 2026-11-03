#ifndef DARKNET_CXX_PORT_PROJECT_DXXWJZ4P_V1_SAX_HANDLER_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_DXXWJZ4P_V1_SAX_HANDLER_CLASS_H

#include <cstdint>
#include <filesystem>
#include <string>

#include "src/NetworksLoaders/dxxwjz/dxxwjz3p/Dxxwjz3pSaxHandler.h"
#include "src/NetworksLoaders/dxxwjz/dxxwjz4/Dxxwjz4Description.h"

namespace darknet_adaptor::loaders::dxxwjz4p::v1
{

/**
 * @brief Builds the document of the SAX events of the dxxwjz4p JSON text (see
 * the Dxxwjz4pFormat) the way the dxxwjz3p handler does, the description
 * text of the description member.
 */
class Dxxwjz4pSaxHandler : public dxxwjz3p::v1::Dxxwjz3pSaxHandler
{
 public:
  virtual ~Dxxwjz4pSaxHandler() = default;

  /// @param directory The folder of the unpacked project, of the relative db
  /// paths.
  Dxxwjz4pSaxHandler(WeightsDocument& document, WeightsReport& report,
                     std::filesystem::path directory, bool npeek = false);

 protected:
  const char* format_name() const override;
  int64_t format_version() const override;
  Delegate* extension(const std::string& name) override;
  bool extension_end(const std::string& name) override;

 private:
  dxxwjz4::v4::Dxxwjz4Description mdescription;
};

}  // namespace darknet_adaptor::loaders::dxxwjz4p::v1

#endif  // DARKNET_CXX_PORT_PROJECT_DXXWJZ4P_V1_SAX_HANDLER_CLASS_H
