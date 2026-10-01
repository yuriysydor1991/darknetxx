#ifndef DARKNET_CXX_PORT_PROJECT_DXXWJZ1_V1_FLOAT_CODEC_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_DXXWJZ1_V1_FLOAT_CODEC_CLASS_H

#include <cstddef>
#include <cstdint>
#include <string>

namespace darknet_adaptor::loaders::dxxwjz1::v1
{

/**
 * @brief The JSON text of the floats of the dxxwjz1 files, which keeps every
 * float bit by bit.
 *
 * The finite float is the JSON number of the shortest decimal text the float
 * round trips of (the "0.1", the "-2.5e-05", the "1e-45" of the smallest
 * denormal), the negative zero is the "-0.0" one, which the JSON readers
 * don't take for the integer zero. The infinities and the NaNs are the JSON
 * strings of their bit patterns (the "0x7fc00000"), which keep the sign and
 * the payload of the NaNs; the bit pattern string of any float is read as
 * well.
 */
class Dxxwjz1FloatCodec
{
 public:
  /// @brief The size of the buffer the encode() writes into.
  static constexpr const std::size_t MAX_TEXT = 32U;

  /**
   * @brief Writes the JSON text of the value.
   *
   * @param value The float to write.
   * @param first The buffer of the MAX_TEXT chars at least.
   *
   * @return The end of the text written.
   */
  static char* encode(float value, char* first);

  /// @brief The JSON text of the value.
  static std::string encode(float value);

  /**
   * @brief Reads the JSON number text: the decimal of the float range, the
   * exact float nearest to it.
   *
   * @return False for the malformed text and the value out of the float
   * range.
   */
  static bool decode_number(const std::string& text, float& value);

  /**
   * @brief The decode_number() of the text of the given decimal point: the
   * JSON lexer keeps the decimal point of the current locale in the number
   * text (see the decimal_point()).
   */
  static bool decode_number(const std::string& text, float& value, char point);

  /**
   * @brief The decimal point of the current locale: the first byte of it,
   * the one the JSON lexer puts in place of the '.' of the number texts.
   */
  static char decimal_point();

  /**
   * @brief Reads the bit pattern string (without the quotes): the "0x" and
   * the 8 hexadecimal digits of either case.
   *
   * @return False for anything else.
   */
  static bool decode_bits(const std::string& text, float& value);

  /// @brief The "0x" bit pattern text of the value (without the quotes).
  static std::string bits_text(float value);

  /// @brief The bit pattern of the value.
  static uint32_t bits_of(float value);
};

}  // namespace darknet_adaptor::loaders::dxxwjz1::v1

#endif  // DARKNET_CXX_PORT_PROJECT_DXXWJZ1_V1_FLOAT_CODEC_CLASS_H
