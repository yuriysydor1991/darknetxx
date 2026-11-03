#ifndef DARKNET_CXX_PORT_PROJECT_DXXWJZ4P_V1_JSON_EMITTER_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_DXXWJZ4P_V1_JSON_EMITTER_CLASS_H

#include <cstdint>
#include <filesystem>
#include <ostream>
#include <string>

#include "src/NetworksLoaders/dxxwjz/dxxwjz3p/Dxxwjz3pJsonEmitter.h"
#include "src/NetworksLoaders/weights-model/Yolo4Annotations.h"

namespace darknet_adaptor::loaders::dxxwjz4p::v1
{

/**
 * @brief Writes the JSON text of the dxxwjz4p document (see the
 * Dxxwjz4pFormat): the dxxwjz3p one of the description member of the dxxwjz4
 * files before the network one.
 */
class Dxxwjz4pJsonEmitter : public dxxwjz3p::v1::Dxxwjz3pJsonEmitter
{
 public:
  virtual ~Dxxwjz4pJsonEmitter() = default;

  /// @param annotations The annotations of the images of the archive paths
  /// of the directory, which outlive the emitter.
  /// @param directory The root of the archive, of the relative db paths.
  /// @param description The text written, which outlives the emitter.
  Dxxwjz4pJsonEmitter(std::ostream& out, const NetworkDescription& network,
                      const TrainingRecord& training,
                      const Yolo4Annotations& annotations,
                      std::filesystem::path directory,
                      const std::string& description);

 protected:
  virtual const char* format_name() const override;
  virtual int64_t format_version() const override;
  /// @brief The description member and the network one up to it's layers.
  virtual void sections() override;

 private:
  const std::string& mdescription;
};

}  // namespace darknet_adaptor::loaders::dxxwjz4p::v1

#endif  // DARKNET_CXX_PORT_PROJECT_DXXWJZ4P_V1_JSON_EMITTER_CLASS_H
