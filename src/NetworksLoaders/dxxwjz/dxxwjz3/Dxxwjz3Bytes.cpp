#include "src/NetworksLoaders/dxxwjz/dxxwjz3/Dxxwjz3Bytes.h"

#include <cstddef>
#include <cstdint>
#include <string>
#include <utility>
#include <vector>

#include "src/NetworksLoaders/dxxwjz/dxxwjz1/Dxxwjz1Format.h"

namespace darknet_adaptor::loaders::dxxwjz3::v3
{

Dxxwjz3Bytes::Dxxwjz3Bytes(std::vector<uint8_t>& nbytes, Failure nfail)
    : mbytes{nbytes}, fail{std::move(nfail)}
{
}

std::string Dxxwjz3Bytes::json_of(const std::vector<uint8_t>& bytes)
{
  std::string json;

  for (const uint8_t byte : bytes) {
    json += (json.empty() ? "[" : ",") + std::to_string(byte);
  }

  return json.empty() ? "[]" : json + "]";
}

bool Dxxwjz3Bytes::null() { return wrong(); }

bool Dxxwjz3Bytes::boolean(bool) { return wrong(); }

bool Dxxwjz3Bytes::number_integer(const number_integer_t value)
{
  return value < 0 ? wrong()
                   : number_unsigned(static_cast<number_unsigned_t>(value));
}

bool Dxxwjz3Bytes::number_unsigned(const number_unsigned_t value)
{
  if (!marray || value > UINT8_MAX) {
    return wrong();
  }

  mbytes.push_back(static_cast<uint8_t>(value));

  return true;
}

bool Dxxwjz3Bytes::number_float(number_float_t, const string_t&)
{
  return wrong();
}

bool Dxxwjz3Bytes::string(string_t&) { return wrong(); }

bool Dxxwjz3Bytes::binary(binary_t&) { return wrong(); }

bool Dxxwjz3Bytes::start_object(std::size_t) { return wrong(); }

bool Dxxwjz3Bytes::key(string_t&) { return wrong(); }

bool Dxxwjz3Bytes::end_object() { return wrong(); }

bool Dxxwjz3Bytes::start_array(std::size_t)
{
  if (marray) {
    return wrong();
  }

  marray = true;
  mbytes.clear();

  return true;
}

bool Dxxwjz3Bytes::end_array()
{
  marray = false;
  return true;
}

bool Dxxwjz3Bytes::parse_error(std::size_t, const std::string&,
                               const nlohmann::detail::exception&)
{
  return false;
}

bool Dxxwjz3Bytes::wrong()
{
  return fail("\"" + std::string{dxxwjz1::v1::Dxxwjz1Format::TRAILING_KEY} +
              "\" isn't the array of the bytes, the integers from 0 to 255");
}

}  // namespace darknet_adaptor::loaders::dxxwjz3::v3
