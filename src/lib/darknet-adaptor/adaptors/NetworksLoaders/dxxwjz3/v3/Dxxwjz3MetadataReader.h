#ifndef DARKNET_CXX_PORT_PROJECT_DXXWJZ3_V3_METADATA_READER_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_DXXWJZ3_V3_METADATA_READER_CLASS_H

#include <string>

#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz2/v2/Dxxwjz2MetadataReader.h"

namespace darknet_adaptor::loaders::dxxwjz3::v3
{

/**
 * @brief Reads the network member of the dxxwjz3 document (see the
 * Dxxwjz3Format) of it's JSON tree into the cfg text of the darknet: the
 * [net] section of the net object, a section of every layer object, a line
 * of every option. Checks the structure the way the Dxxwjz2MetadataReader
 * does: the option keys and values the darknet reads, the types of the
 * sections, skips the unknown keys with a warning. The training member has
 * no iteration column, the losses are the ones of their indexes.
 */
class Dxxwjz3MetadataReader : public dxxwjz2::v2::Dxxwjz2MetadataReader
{
 public:
  virtual ~Dxxwjz3MetadataReader() = default;
  explicit Dxxwjz3MetadataReader(WeightsReport& nreport);

  virtual bool network(const json& value,
                       NetworkDescription& description) override;
  virtual bool training(const json& value, TrainingRecord& record) override;

 private:
  bool layer(const json& value, const std::string& path, std::string& cfg);

  /// @brief Appends the cfg section of the type and of the options object.
  bool section(const std::string& type, const json& options,
               const std::string& path, std::string& cfg);
};

}  // namespace darknet_adaptor::loaders::dxxwjz3::v3

#endif  // DARKNET_CXX_PORT_PROJECT_DXXWJZ3_V3_METADATA_READER_CLASS_H
