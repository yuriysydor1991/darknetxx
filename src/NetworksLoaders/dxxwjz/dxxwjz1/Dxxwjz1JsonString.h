#ifndef DARKNET_CXX_PORT_PROJECT_DXXWJZ1_V1_JSON_STRING_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_DXXWJZ1_V1_JSON_STRING_CLASS_H

#include <string>

namespace darknet_adaptor::loaders::dxxwjz1::v1
{

/// @brief The JSON string text of any text.
class Dxxwjz1JsonString
{
 public:
  /**
   * @brief The quoted and escaped text. The JSON text is UTF-8, so the bytes
   * of the text which aren't (the Latin-1 comment of a cfg file, for
   * example) are replaced with the U+FFFD replacement characters.
   */
  static std::string quote(const std::string& text);

 private:
  /// @brief The length of the valid UTF-8 sequence at the position, 0 for
  /// none.
  static std::size_t sequence(const std::string& text, std::size_t position);

  static void escape(char symbol, std::string& result);
};

}  // namespace darknet_adaptor::loaders::dxxwjz1::v1

#endif  // DARKNET_CXX_PORT_PROJECT_DXXWJZ1_V1_JSON_STRING_CLASS_H
