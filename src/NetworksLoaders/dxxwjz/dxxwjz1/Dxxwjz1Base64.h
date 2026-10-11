#ifndef DARKNET_CXX_PORT_PROJECT_DXXWJZ1_V1_BASE64_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_DXXWJZ1_V1_BASE64_CLASS_H

#include <cstdint>
#include <string>
#include <vector>

namespace darknet_adaptor::loaders::dxxwjz1::v1
{

/**
 * @brief The base64 text (RFC 4648, the padded one of the standard alphabet)
 * of the bytes the dxxwjz1 file keeps as the JSON string: the trailing bytes
 * of the original weights file.
 */
class Dxxwjz1Base64
{
 public:
  static std::string encode(const std::vector<uint8_t>& bytes);

  /**
   * @brief Reads the padded base64 text.
   *
   * @return False for the text of the length other than the multiple of 4,
   * of the characters out of the alphabet or of the misplaced padding.
   */
  static bool decode(const std::string& text, std::vector<uint8_t>& bytes);
};

}  // namespace darknet_adaptor::loaders::dxxwjz1::v1

#endif  // DARKNET_CXX_PORT_PROJECT_DXXWJZ1_V1_BASE64_CLASS_H
