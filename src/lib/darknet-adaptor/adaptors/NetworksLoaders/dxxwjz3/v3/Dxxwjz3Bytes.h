#ifndef DARKNET_CXX_PORT_PROJECT_DXXWJZ3_V3_BYTES_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_DXXWJZ3_V3_BYTES_CLASS_H

#include <cstddef>
#include <cstdint>
#include <functional>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

namespace darknet_adaptor::loaders::dxxwjz3::v3
{

/**
 * @brief The JSON array of the trailing bytes of the dxxwjz3 files (see the
 * Dxxwjz3Format): the integers from 0 to 255. Builds the bytes of the SAX
 * events of the array a value at a time, so no JSON tree of the bytes the
 * original weights file of the other network keeps (the megabytes of them)
 * is made.
 */
class Dxxwjz3Bytes : public nlohmann::json_sax<nlohmann::json>
{
 public:
  /// @brief Takes the error message, returns false.
  using Failure = std::function<bool(const std::string&)>;

  virtual ~Dxxwjz3Bytes() = default;

  /// @param nbytes The bytes built, which outlive the builder.
  Dxxwjz3Bytes(std::vector<uint8_t>& nbytes, Failure nfail);

  /// @brief The JSON array text of the bytes.
  static std::string json_of(const std::vector<uint8_t>& bytes);

  bool null() override;
  bool boolean(bool value) override;
  bool number_integer(number_integer_t value) override;
  bool number_unsigned(number_unsigned_t value) override;
  bool number_float(number_float_t value, const string_t& text) override;
  bool string(string_t& text) override;
  bool binary(binary_t& value) override;
  bool start_object(std::size_t elements) override;
  bool key(string_t& name) override;
  bool end_object() override;
  bool start_array(std::size_t elements) override;
  bool end_array() override;
  bool parse_error(std::size_t position, const std::string& last_token,
                   const nlohmann::detail::exception& ex) override;

 private:
  /// @brief Fails the value of no byte.
  bool wrong();

  std::vector<uint8_t>& mbytes;
  Failure fail;
  bool marray{false};
};

}  // namespace darknet_adaptor::loaders::dxxwjz3::v3

#endif  // DARKNET_CXX_PORT_PROJECT_DXXWJZ3_V3_BYTES_CLASS_H
