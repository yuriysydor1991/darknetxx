#ifndef DARKNET_CXX_PORT_PROJECT_DXXWJZ2_V2_JSON_EMITTER_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_DXXWJZ2_V2_JSON_EMITTER_CLASS_H

#include <cstdint>
#include <ostream>
#include <string>
#include <vector>

#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz1/v1/Dxxwjz1JsonEmitter.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/weights-model/NetworkDescription.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/weights-model/TrainingRecord.h"

namespace darknet_adaptor::loaders::dxxwjz2::v2
{

/**
 * @brief Writes the JSON text of the dxxwjz2 document (see the Dxxwjz2Format):
 * the dxxwjz1 one with the network and the training members, a class on a
 * line and a column of the losses on a line.
 */
class Dxxwjz2JsonEmitter : public dxxwjz1::v1::Dxxwjz1JsonEmitter
{
 public:
  virtual ~Dxxwjz2JsonEmitter() = default;

  /// @param network The description written, which outlives the emitter.
  /// @param training The record written, which outlives the emitter.
  Dxxwjz2JsonEmitter(std::ostream& out, const NetworkDescription& network,
                     const TrainingRecord& training);

 protected:
  virtual const char* format_name() const override;
  virtual int64_t format_version() const override;
  virtual void sections() override;

  /// @brief The members of the network description before its classes: the
  /// cfg text member here.
  virtual std::string cfg(const NetworkDescription& network) const;

  /// @brief The loss columns of the training record: the iteration, the
  /// loss and the avg_loss ones here.
  virtual void losses(const TrainingRecord& training);

  /// @brief The loss and the avg_loss columns.
  void columns(const std::vector<float>& losses,
               const std::vector<float>& avg_losses);

 private:
  void network();
  void training();
  void integers(const std::vector<int>& values);

  const NetworkDescription& mnetwork;
  const TrainingRecord& mtraining;
};

}  // namespace darknet_adaptor::loaders::dxxwjz2::v2

#endif  // DARKNET_CXX_PORT_PROJECT_DXXWJZ2_V2_JSON_EMITTER_CLASS_H
