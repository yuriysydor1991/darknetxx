#ifndef DARKNET_CXX_PORT_PROJECT_DXXWJZ4_V4_JSON_EMITTER_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_DXXWJZ4_V4_JSON_EMITTER_CLASS_H

#include <cstdint>
#include <ostream>
#include <string>

#include "src/NetworksLoaders/dxxwjz/dxxwjz3/Dxxwjz3JsonEmitter.h"

namespace darknet_adaptor::loaders::dxxwjz4::v4
{

/**
 * @brief Writes the JSON text of the dxxwjz4 document (see the
 * Dxxwjz4Format): the dxxwjz3 one of the description member before the
 * network one.
 */
class Dxxwjz4JsonEmitter : public dxxwjz3::v3::Dxxwjz3JsonEmitter
{
 public:
  virtual ~Dxxwjz4JsonEmitter() = default;

  /// @param description The text written, which outlives the emitter.
  Dxxwjz4JsonEmitter(std::ostream& out, const NetworkDescription& network,
                     const TrainingRecord& training,
                     const std::string& description);

 protected:
  virtual const char* format_name() const override;
  virtual int64_t format_version() const override;
  /// @brief The description member and the network one up to it's layers.
  virtual void sections() override;

 private:
  const std::string& mdescription;
};

}  // namespace darknet_adaptor::loaders::dxxwjz4::v4

#endif  // DARKNET_CXX_PORT_PROJECT_DXXWJZ4_V4_JSON_EMITTER_CLASS_H
