#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz2/v2/Dxxwjz2SubtreeBuilder.h"

#include <cstddef>
#include <string>
#include <utility>

#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz1/v1/Dxxwjz1FloatCodec.h"

namespace darknet_adaptor::loaders::dxxwjz2::v2
{

using dxxwjz1::v1::Dxxwjz1FloatCodec;

Dxxwjz2SubtreeBuilder::Dxxwjz2SubtreeBuilder(Failure nfail)
    : fail{std::move(nfail)}, mpoint{Dxxwjz1FloatCodec::decimal_point()}
{
}

void Dxxwjz2SubtreeBuilder::reset(const std::string& name)
{
  mtree = json{};
  mstack.clear();
  mpaths.assign(1U, name);
  mkey.clear();
}

const Dxxwjz2SubtreeBuilder::json& Dxxwjz2SubtreeBuilder::tree() const
{
  return mtree;
}

bool Dxxwjz2SubtreeBuilder::null() { return add(json{}, false); }

bool Dxxwjz2SubtreeBuilder::boolean(const bool value)
{
  return add(json(value), false);
}

bool Dxxwjz2SubtreeBuilder::number_integer(const number_integer_t value)
{
  return add(json(value), false);
}

bool Dxxwjz2SubtreeBuilder::number_unsigned(const number_unsigned_t value)
{
  return add(json(value), false);
}

bool Dxxwjz2SubtreeBuilder::number_float(number_float_t, const string_t& text)
{
  float value = 0.f;

  if (!Dxxwjz1FloatCodec::decode_number(text, value, mpoint)) {
    return fail(path() + " " + text + " is out of the float range");
  }

  return add(json(static_cast<double>(value)), false);
}

bool Dxxwjz2SubtreeBuilder::string(string_t& text)
{
  return add(json(text), false);
}

bool Dxxwjz2SubtreeBuilder::binary(binary_t&)
{
  return fail(path() + " is the binary value");
}

bool Dxxwjz2SubtreeBuilder::start_object(std::size_t)
{
  return add(json::object(), true);
}

bool Dxxwjz2SubtreeBuilder::key(string_t& name)
{
  mkey = name;
  return true;
}

bool Dxxwjz2SubtreeBuilder::end_object()
{
  json& value = *mstack.back();

  mstack.pop_back();
  mpaths.pop_back();

  return closed(value);
}

bool Dxxwjz2SubtreeBuilder::start_array(std::size_t)
{
  return add(json::array(), true);
}

bool Dxxwjz2SubtreeBuilder::end_array() { return end_object(); }

bool Dxxwjz2SubtreeBuilder::parse_error(std::size_t, const std::string&,
                                        const nlohmann::detail::exception&)
{
  return false;
}

bool Dxxwjz2SubtreeBuilder::closed(json&) { return true; }

bool Dxxwjz2SubtreeBuilder::add(json value, const bool container)
{
  // the path of the container alone, which the values of the big arrays
  // don't take the time of
  const std::string inside = container ? path(false) : std::string{};
  json* slot = &mtree;

  if (mstack.empty()) {
    mtree = std::move(value);
  } else if (mstack.back()->is_object()) {
    if (mstack.back()->contains(mkey)) {
      return fail("the key " + path() + " is given twice");
    }

    slot = &((*mstack.back())[mkey] = std::move(value));
  } else {
    mstack.back()->push_back(std::move(value));
    slot = &mstack.back()->back();
  }

  if (container) {
    mstack.push_back(slot);
    mpaths.push_back(inside);
  }

  return true;
}

std::string Dxxwjz2SubtreeBuilder::path(const bool quoted) const
{
  if (mstack.empty()) {
    return mpaths.front();
  }

  if (mstack.back()->is_object()) {
    return mpaths.back() + (quoted ? ".\"" + mkey + "\"" : "." + mkey);
  }

  return mpaths.back() + "[" + std::to_string(mstack.back()->size()) + "]";
}

}  // namespace darknet_adaptor::loaders::dxxwjz2::v2
