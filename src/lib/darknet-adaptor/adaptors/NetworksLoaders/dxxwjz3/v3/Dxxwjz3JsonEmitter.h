#ifndef DARKNET_CXX_PORT_PROJECT_DXXWJZ3_V3_JSON_EMITTER_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_DXXWJZ3_V3_JSON_EMITTER_CLASS_H

#include <cstddef>
#include <cstdint>
#include <ostream>
#include <string>
#include <vector>

#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz2/v2/Dxxwjz2JsonEmitter.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz3/v3/Dxxwjz3CfgJson.h"

namespace darknet_adaptor::loaders::dxxwjz3::v3
{

/**
 * @brief Writes the JSON text of the dxxwjz3 document (see the
 * Dxxwjz3Format): the dxxwjz2 one of the network member of the JSON of the
 * cfg (see the Dxxwjz3CfgJson), every layer object of it's arrays, of the
 * array of the class names and of the array of the trailing bytes. The
 * layers of no layer() keep no arrays.
 */
class Dxxwjz3JsonEmitter : public dxxwjz2::v2::Dxxwjz2JsonEmitter
{
 public:
  virtual ~Dxxwjz3JsonEmitter() = default;

  /// @param network The description written, of the cfg the file keeps
  /// (see the Dxxwjz3Writer::keeps()), which outlives the emitter.
  /// @param training The record written, which outlives the emitter.
  Dxxwjz3JsonEmitter(std::ostream& out, const NetworkDescription& network,
                     const TrainingRecord& training);

  /// @brief The format, the version, the generator and the network member
  /// up to it's layers.
  virtual void begin(const WeightsHeader& header, bool weights = true) override;

  /// @brief The layer of the index of the cfg ones with it's arrays, after
  /// the layers before it.
  virtual void layer(
      int index, LAYER_TYPE type,
      const std::vector<dxxwjz1::v1::Dxxwjz1ArrayView>& arrays) override;

  /// @brief The rest of the layers and of the network member, the members()
  /// and, for the weights, the header and the trailing bytes.
  virtual void end(const std::vector<uint8_t>& bytes) override;

 protected:
  virtual const char* format_name() const override;
  virtual int64_t format_version() const override;
  /// @brief The network member up to it's layers.
  virtual void sections() override;
  virtual std::string trailing(
      const std::vector<uint8_t>& bytes) const override;
  /// @brief The losses of the iterations of their indexes, of no iteration
  /// column (see the Dxxwjz3Losses).
  virtual void losses(const TrainingRecord& training) override;

  /// @brief The members after the network one: the training one here.
  virtual void members();

 private:
  /// @brief The layers of no arrays up to the index.
  void layers_before(std::size_t index);

  /// @brief The classes member: the array of the names at their indexes.
  void class_names();

  const Dxxwjz3CfgJson mcfg;
  const std::vector<std::string>& mclasses;
  WeightsHeader mheader;
  bool mweights{true};
  /// the count of the layers written
  std::size_t mlayers{0U};
};

}  // namespace darknet_adaptor::loaders::dxxwjz3::v3

#endif  // DARKNET_CXX_PORT_PROJECT_DXXWJZ3_V3_JSON_EMITTER_CLASS_H
