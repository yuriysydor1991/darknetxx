#ifndef DARKNET_CXX_PORT_PROJECT_DXXWJZ3_V3_VALUE_CODEC_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_DXXWJZ3_V3_VALUE_CODEC_CLASS_H

#include <nlohmann/json.hpp>
#include <string>

namespace darknet_adaptor::loaders::dxxwjz3::v3
{

/**
 * @brief The JSON values of the network cfg options of the dxxwjz3 files
 * (see the Dxxwjz3Format): the option text split at the commas into the
 * array of its items, the number item is the JSON number, the rest are the
 * JSON strings.
 *
 * The number is the one of the item darknet reads the same way: the atoi()
 * and the atof() of the item and of the number are the same, so the ".1"
 * is the 0.1, while the "1e-3" stays the string (the atoi() of it is 1). The
 * integers are of the 64 bits range, the fractions are the shortest fixed
 * notation decimals of their floats, the darknet keeps the floats of the
 * options.
 */
class Dxxwjz3ValueCodec
{
 public:
  using json = nlohmann::ordered_json;

  /// @brief The JSON text of the option value, the null of the NULL value
  /// (of the option line without the '=').
  static std::string json_of(const char* value);

  /**
   * @brief The cfg text of the JSON value: the number, the string or the
   * array of them joined by the commas.
   *
   * @return False for the value of no such text: the empty one, the string
   * of the white space (the darknet drops it), the rest of the types.
   */
  static bool text_of(const json& value, std::string& text);

 private:
  static std::string item_json(const std::string& item);
  static bool item_text(const json& item, std::string& text);

  /// @brief The shortest fixed notation decimal the float round trips of.
  static std::string fraction_text(float value);
};

}  // namespace darknet_adaptor::loaders::dxxwjz3::v3

#endif  // DARKNET_CXX_PORT_PROJECT_DXXWJZ3_V3_VALUE_CODEC_CLASS_H
