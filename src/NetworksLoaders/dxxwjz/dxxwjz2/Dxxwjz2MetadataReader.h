#ifndef DARKNET_CXX_PORT_PROJECT_DXXWJZ2_V2_METADATA_READER_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_DXXWJZ2_V2_METADATA_READER_CLASS_H

#include <initializer_list>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

#include "src/NetworksLoaders/weights-model/NetworkDescription.h"
#include "src/NetworksLoaders/weights-model/TrainingRecord.h"
#include "src/NetworksLoaders/weights-model/WeightsReport.h"

namespace darknet_adaptor::loaders::dxxwjz2::v2
{

/**
 * @brief Reads the network and the training members of the dxxwjz2 document
 * (see the Dxxwjz2Format) of their JSON trees (see the
 * Dxxwjz2SubtreeBuilder): checks their structure, skips the unknown keys
 * with a warning.
 */
class Dxxwjz2MetadataReader
{
 public:
  /// @brief The trees of the keys in the file order.
  using json = nlohmann::ordered_json;

  virtual ~Dxxwjz2MetadataReader() = default;
  explicit Dxxwjz2MetadataReader(WeightsReport& nreport);

  /// @brief The value of the network key, false for the wrong one (see the
  /// error()).
  virtual bool network(const json& value, NetworkDescription& description);

  /// @brief The value of the training key, false for the wrong one.
  virtual bool training(const json& value, TrainingRecord& record);

  const std::string& error() const;

 protected:
  /// @brief The optional map columns of the training value.
  bool maps(const json& value, TrainingRecord& record);
  bool seconds(const json& value, double& result);
  bool floats(const json& value, const std::string& path,
              std::vector<float>& column);

  /// @brief Checks the value to be an object of the required keys, warns
  /// of the unknown ones.
  bool object(const json& value, const std::string& path,
              std::initializer_list<const char*> required,
              std::initializer_list<const char*> known);

  static std::string member(const std::string& path, const char* name);

  bool fail(const std::string& message);

  /// @brief The value of the classes key of the network.
  bool classes(const json& value, std::vector<std::string>& names);

 private:
  bool class_of(const json& value, const std::string& path,
                std::vector<std::string>& names, std::vector<bool>& given);
  bool integers(const json& value, const std::string& path,
                std::vector<int>& column);

  WeightsReport& mreport;
  std::string merror;
};

}  // namespace darknet_adaptor::loaders::dxxwjz2::v2

#endif  // DARKNET_CXX_PORT_PROJECT_DXXWJZ2_V2_METADATA_READER_CLASS_H
