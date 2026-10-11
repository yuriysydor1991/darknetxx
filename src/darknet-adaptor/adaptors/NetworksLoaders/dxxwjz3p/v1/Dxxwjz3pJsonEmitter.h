#ifndef DARKNET_CXX_PORT_PROJECT_DXXWJZ3P_V1_JSON_EMITTER_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_DXXWJZ3P_V1_JSON_EMITTER_CLASS_H

#include <cstdint>
#include <filesystem>
#include <ostream>

#include "src/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz2p/v1/Dxxwjz2pAnnotationsJson.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz3/v3/Dxxwjz3JsonEmitter.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/weights-model/Yolo4Annotations.h"

namespace darknet_adaptor::loaders::dxxwjz3p::v1
{

/**
 * @brief Writes the JSON text of the dxxwjz3p document (see the
 * Dxxwjz3pFormat): the dxxwjz3 one with the annotations member of the
 * dxxwjz2p projects after the training one (see the Dxxwjz2pAnnotationsJson).
 */
class Dxxwjz3pJsonEmitter : public dxxwjz3::v3::Dxxwjz3JsonEmitter
{
 public:
  virtual ~Dxxwjz3pJsonEmitter() = default;

  /// @param annotations The annotations written of the classes of the
  /// network, both outlive the emitter.
  /// @param directory The directory of the file, of the relative db paths.
  Dxxwjz3pJsonEmitter(std::ostream& out, const NetworkDescription& network,
                      const TrainingRecord& training,
                      const Yolo4Annotations& annotations,
                      std::filesystem::path directory);

 protected:
  virtual const char* format_name() const override;
  virtual int64_t format_version() const override;
  /// @brief The training and the annotations members.
  virtual void members() override;

 private:
  dxxwjz2p::v1::Dxxwjz2pAnnotationsJson mannotations;
};

}  // namespace darknet_adaptor::loaders::dxxwjz3p::v1

#endif  // DARKNET_CXX_PORT_PROJECT_DXXWJZ3P_V1_JSON_EMITTER_CLASS_H
