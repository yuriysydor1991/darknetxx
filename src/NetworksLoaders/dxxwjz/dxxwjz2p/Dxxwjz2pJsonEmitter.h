#ifndef DARKNET_CXX_PORT_PROJECT_DXXWJZ2P_V1_JSON_EMITTER_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_DXXWJZ2P_V1_JSON_EMITTER_CLASS_H

#include <cstdint>
#include <filesystem>
#include <ostream>

#include "src/NetworksLoaders/dxxwjz/dxxwjz2/Dxxwjz2JsonEmitter.h"
#include "src/NetworksLoaders/dxxwjz/dxxwjz2p/Dxxwjz2pAnnotationsJson.h"
#include "src/NetworksLoaders/weights-model/Yolo4Annotations.h"

namespace darknet_adaptor::loaders::dxxwjz2p::v1
{

/**
 * @brief Writes the JSON text of the dxxwjz2p document (see the
 * Dxxwjz2pFormat): the dxxwjz2 one with the annotations member before the
 * weights (see the Dxxwjz2pAnnotationsJson).
 */
class Dxxwjz2pJsonEmitter : public dxxwjz2::v2::Dxxwjz2JsonEmitter
{
 public:
  virtual ~Dxxwjz2pJsonEmitter() = default;

  /// @param annotations The annotations written of the classes of the
  /// network, both outlive the emitter.
  /// @param directory The directory of the file, of the relative db paths.
  Dxxwjz2pJsonEmitter(std::ostream& out, const NetworkDescription& network,
                      const TrainingRecord& training,
                      const Yolo4Annotations& annotations,
                      std::filesystem::path directory);

 protected:
  virtual const char* format_name() const override;
  virtual int64_t format_version() const override;
  virtual void sections() override;

 private:
  Dxxwjz2pAnnotationsJson mannotations;
};

}  // namespace darknet_adaptor::loaders::dxxwjz2p::v1

#endif  // DARKNET_CXX_PORT_PROJECT_DXXWJZ2P_V1_JSON_EMITTER_CLASS_H
