#ifndef DARKNET_CXX_PORT_PROJECT_DXXWJZ4_V4_DESCRIPTION_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_DXXWJZ4_V4_DESCRIPTION_CLASS_H

#include <nlohmann/json.hpp>
#include <string>

#include "src/NetworksLoaders/dxxwjz/dxxwjz2/Dxxwjz2SubtreeBuilder.h"

namespace darknet_adaptor::loaders::dxxwjz4::v4
{

/**
 * @brief The description member of the dxxwjz4 and the dxxwjz4p documents
 * (see the Dxxwjz4Format): the JSON text of the member the emitters write
 * and the read of the value the SAX handlers hand over to it.
 */
class Dxxwjz4Description
{
 public:
  using Delegate = nlohmann::json_sax<nlohmann::json>;

  virtual ~Dxxwjz4Description() = default;

  /// @param nfail Takes the error message of the value, returns false.
  explicit Dxxwjz4Description(
      dxxwjz2::v2::Dxxwjz2SubtreeBuilder::Failure nfail);

  /// @brief The JSON text of the member: "\"description\": \"...\"".
  static std::string member(const std::string& text);

  /// @brief Starts the value of the member, the handler of it's events.
  Delegate* start();

  /**
   * @brief Reads the value of the member once it's complete.
   *
   * @param description Out: the text of the value.
   *
   * @return False for the value which isn't the string (failed).
   */
  bool read(std::string& description);

 private:
  dxxwjz2::v2::Dxxwjz2SubtreeBuilder::Failure fail;
  dxxwjz2::v2::Dxxwjz2SubtreeBuilder mbuilder;
};

}  // namespace darknet_adaptor::loaders::dxxwjz4::v4

#endif  // DARKNET_CXX_PORT_PROJECT_DXXWJZ4_V4_DESCRIPTION_CLASS_H
