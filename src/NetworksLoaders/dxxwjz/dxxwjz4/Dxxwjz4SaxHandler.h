#ifndef DARKNET_CXX_PORT_PROJECT_DXXWJZ4_V4_SAX_HANDLER_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_DXXWJZ4_V4_SAX_HANDLER_CLASS_H

#include <cstdint>
#include <string>

#include "src/NetworksLoaders/dxxwjz/dxxwjz3/Dxxwjz3SaxHandler.h"
#include "src/NetworksLoaders/dxxwjz/dxxwjz4/Dxxwjz4Description.h"

namespace darknet_adaptor::loaders::dxxwjz4::v4
{

/**
 * @brief Builds the document of the SAX events of the dxxwjz4 JSON text (see
 * the Dxxwjz4Format) the way the dxxwjz3 handler does, the description text
 * of the description member.
 */
class Dxxwjz4SaxHandler : public dxxwjz3::v3::Dxxwjz3SaxHandler
{
 public:
  virtual ~Dxxwjz4SaxHandler() = default;

  Dxxwjz4SaxHandler(WeightsDocument& document, WeightsReport& report,
                    bool npeek = false);

 protected:
  const char* format_name() const override;
  int64_t format_version() const override;
  Delegate* extension(const std::string& name) override;
  bool extension_end(const std::string& name) override;

 private:
  Dxxwjz4Description mdescription;
};

}  // namespace darknet_adaptor::loaders::dxxwjz4::v4

#endif  // DARKNET_CXX_PORT_PROJECT_DXXWJZ4_V4_SAX_HANDLER_CLASS_H
