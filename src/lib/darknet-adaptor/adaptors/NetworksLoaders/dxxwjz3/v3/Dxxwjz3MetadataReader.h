#ifndef DARKNET_CXX_PORT_PROJECT_DXXWJZ3_V3_METADATA_READER_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_DXXWJZ3_V3_METADATA_READER_CLASS_H

#include <string>
#include <vector>

#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz2/v2/Dxxwjz2MetadataReader.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/weights-model/WeightsDocument.h"

namespace darknet_adaptor::loaders::dxxwjz3::v3
{

/**
 * @brief Reads the network member of the dxxwjz3 document (see the
 * Dxxwjz3Format) of it's JSON tree into the cfg text of the darknet: the
 * [net] section of the net object, a section of every layer object, a line
 * of every option; the arrays of the layer objects into the layers of the
 * document (see the arrays()). Checks the structure the way the
 * Dxxwjz2MetadataReader does: the option keys and values the darknet reads,
 * the types of the sections, the names of the sub-layers and of the arrays,
 * the floats, skips the unknown keys with a warning. The classes are the
 * array of their names, the name of an index is the one of the class of the
 * index. The training member has no iteration column, the losses are the
 * ones of their indexes.
 */
class Dxxwjz3MetadataReader : public dxxwjz2::v2::Dxxwjz2MetadataReader
{
 public:
  virtual ~Dxxwjz3MetadataReader() = default;
  explicit Dxxwjz3MetadataReader(WeightsReport& nreport);

  virtual bool network(const json& value,
                       NetworkDescription& description) override;
  virtual bool training(const json& value, TrainingRecord& record) override;

  /**
   * @brief Moves the arrays of the layer object of the network (the arrays
   * and the sublayers members of the dxxwjz1 layers, see the Dxxwjz1Format)
   * out of it into the layer of the index, of the type of it's cfg section
   * (see the WeightsSchema::section_type()), which the layers get. The layer
   * object of no arrays adds no layer, the value of no object is the one of
   * the network() to check.
   *
   * @return False for the wrong arrays (see the error()).
   */
  bool arrays(json& value, int index, std::vector<WeightsLayer>& layers);

 private:
  bool layer(const json& value, const std::string& path, std::string& cfg);

  /// @brief The class names of the classes array of the network.
  bool names_of(const json& value, std::vector<std::string>& names);

  /// @brief Appends the cfg section of the type and of the options object.
  bool section(const std::string& type, const json& options,
               const std::string& path, std::string& cfg);

  /// @brief Appends the arrays of the sub-layers of the sublayers object.
  bool sublayers_of(const json& value, const std::string& path,
                    std::vector<WeightsArray>& arrays);

  /// @brief Appends the arrays of the arrays object of the sub-layer.
  bool arrays_of(const json& value, const std::string& path,
                 WeightsSubLayer sub, std::vector<WeightsArray>& arrays);
};

}  // namespace darknet_adaptor::loaders::dxxwjz3::v3

#endif  // DARKNET_CXX_PORT_PROJECT_DXXWJZ3_V3_METADATA_READER_CLASS_H
