#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz1/v1/Dxxwjz1SaxHandler.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <string>
#include <utility>
#include <vector>

#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz1/v1/Dxxwjz1Base64.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz1/v1/Dxxwjz1FloatCodec.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz1/v1/Dxxwjz1Format.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/weights-model/WeightsDocument.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/weights-model/WeightsReport.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/weights-model/WeightsSchema.h"

namespace darknet_adaptor::loaders::dxxwjz1::v1
{

Dxxwjz1SaxHandler::Dxxwjz1SaxHandler(WeightsDocument& document,
                                     WeightsReport& report)
    : mdocument{document}, mreport{report}
{
}

bool Dxxwjz1SaxHandler::null()
{
  if (mdelegate != nullptr) {
    return delegated_value(mdelegate->null());
  }

  return scalar("null");
}

bool Dxxwjz1SaxHandler::boolean(const bool value)
{
  if (mdelegate != nullptr) {
    return delegated_value(mdelegate->boolean(value));
  }

  return scalar("boolean");
}

bool Dxxwjz1SaxHandler::number_integer(const number_integer_t value)
{
  if (mdelegate != nullptr) {
    return delegated_value(mdelegate->number_integer(value));
  }

  if (mstack.empty()) {
    return not_an_object();
  }

  if (skip_value()) {
    return true;
  }

  if (context() == Context::Values) {
    return append(static_cast<float>(value));
  }

  const Field field = take_field();

  switch (field) {
    case Field::Version:
      return version(value);
    case Field::Major:
    case Field::Minor:
    case Field::Revision:
      return header_value(field, value);
    case Field::Seen:
      if (value < 0) {
        return fail(where_of(field) + " is negative");
      }
      mdocument.header.seen = static_cast<uint64_t>(value);
      return true;
    case Field::Index:
      return layer_index(value);
    default:
      return wrong_type(field, "integer");
  }
}

bool Dxxwjz1SaxHandler::number_unsigned(const number_unsigned_t value)
{
  if (mdelegate != nullptr) {
    return delegated_value(mdelegate->number_unsigned(value));
  }

  if (mstack.empty()) {
    return not_an_object();
  }

  if (skip_value()) {
    return true;
  }

  if (context() == Context::Values) {
    return append(static_cast<float>(value));
  }

  const Field field = take_field();

  if (field == Field::Seen) {
    mdocument.header.seen = value;
    return true;
  }

  if (value > static_cast<number_unsigned_t>(
                  std::numeric_limits<number_integer_t>::max())) {
    return fail(where_of(field) + " is out of the range");
  }

  restore_field(field);

  return number_integer(static_cast<number_integer_t>(value));
}

bool Dxxwjz1SaxHandler::number_float(const number_float_t value,
                                     const string_t& text)
{
  if (mdelegate != nullptr) {
    return delegated_value(mdelegate->number_float(value, text));
  }

  if (mstack.empty()) {
    return not_an_object();
  }

  if (skip_value()) {
    return true;
  }

  if (context() == Context::Values) {
    float parsed = 0.f;

    if (!Dxxwjz1FloatCodec::decode_number(text, parsed, mpoint)) {
      return fail(where_value() + " " + text +
                  " is out of the float range (write the non finite floats "
                  "as their 0x bit pattern strings)");
    }

    return append(parsed);
  }

  return wrong_type(take_field(), "fraction");
}

bool Dxxwjz1SaxHandler::string(string_t& text)
{
  if (mdelegate != nullptr) {
    return delegated_value(mdelegate->string(text));
  }

  if (mstack.empty()) {
    return not_an_object();
  }

  if (skip_value()) {
    return true;
  }

  if (context() == Context::Values) {
    float value = 0.f;

    if (!Dxxwjz1FloatCodec::decode_bits(text, value)) {
      return fail(where_value() + " \"" + text +
                  "\" isn't the 0x bit pattern of a float");
    }

    return append(value);
  }

  const Field field = take_field();

  switch (field) {
    case Field::Format:
      if (text != format_name()) {
        return fail("the format \"" + text + "\" isn't the " + format_name() +
                    " one");
      }
      return true;
    case Field::Generator:
      return true;
    case Field::Trailing:
      if (!Dxxwjz1Base64::decode(text, mdocument.trailing)) {
        return fail(where_of(field) + " isn't the base64 text");
      }
      return true;
    case Field::Type:
      if (!WeightsSchema::parse(text, layer().type)) {
        return fail(where_of(field) + " \"" + text +
                    "\" isn't a darknet layer type");
      }
      return true;
    default:
      return wrong_type(field, "string");
  }
}

bool Dxxwjz1SaxHandler::binary(binary_t& value)
{
  if (mdelegate != nullptr) {
    return delegated_value(mdelegate->binary(value));
  }

  return scalar("binary");
}

bool Dxxwjz1SaxHandler::start_object(const std::size_t elements)
{
  if (mdelegate != nullptr) {
    return delegated_start(mdelegate->start_object(elements));
  }

  if (mstack.empty()) {
    if (mdone) {
      return fail("the text holds more than one document");
    }

    push(Context::Document, "");
    return true;
  }

  if (in_skipped() || mfield == Field::Skip) {
    push(Context::Skip, "");
    return true;
  }

  if (context() == Context::Layers) {
    mdocument.layers.emplace_back();
    mdocument.layers.back().index = -1;
    push(Context::Layer,
         "[" + std::to_string(mdocument.layers.size() - 1U) + "]");
    return true;
  }

  if (context() == Context::Values) {
    return fail(where_value() + " is an object, not a float");
  }

  const Field field = take_field();

  switch (field) {
    case Field::Header:
      push(Context::Header, Dxxwjz1Format::HEADER_KEY);
      return true;
    case Field::Arrays:
      push(Context::Arrays, Dxxwjz1Format::ARRAYS_KEY, mpending_sub);
      return true;
    case Field::Sublayers:
      push(Context::Sublayers, Dxxwjz1Format::SUBLAYERS_KEY);
      return true;
    case Field::Sublayer:
      push(Context::Sublayer, WeightsSchema::name(mpending_sub), mpending_sub);
      return true;
    case Field::Skip:
      push(Context::Skip, "");
      return true;
    default:
      return wrong_type(field, "object");
  }
}

bool Dxxwjz1SaxHandler::key(string_t& name)
{
  if (mdelegate != nullptr) {
    return mdelegate->key(name);
  }

  if (in_skipped()) {
    return true;
  }

  Frame& frame = mstack.back();

  if (std::find(frame.keys.begin(), frame.keys.end(), name) !=
      frame.keys.end()) {
    return fail("the key " + join(name) + " is given twice");
  }

  frame.keys.push_back(name);

  mkey = name;
  mfield = field_of(frame, name);

  if (mfield == Field::None) {
    return false;
  }

  if (mfield == Field::Skip && frame.context == Context::Document) {
    mdelegate = extension(name);

    if (mdelegate != nullptr) {
      mdelegate_key = name;
      mdelegate_depth = 0U;
      mfield = Field::None;
      return true;
    }
  }

  if (mfield == Field::Skip) {
    mreport.warning("the unknown key " + join(name) + " is skipped");
  }

  return true;
}

bool Dxxwjz1SaxHandler::end_object()
{
  if (mdelegate != nullptr) {
    return delegated_end(mdelegate->end_object());
  }

  const Frame frame = mstack.back();

  mstack.pop_back();

  switch (frame.context) {
    case Context::Document:
      mdone = true;
      return require(frame, required_keys());
    case Context::Header:
      return require(frame,
                     {Dxxwjz1Format::MAJOR_KEY, Dxxwjz1Format::MINOR_KEY,
                      Dxxwjz1Format::REVISION_KEY, Dxxwjz1Format::SEEN_KEY});
    case Context::Layer:
      return require(frame,
                     {Dxxwjz1Format::INDEX_KEY, Dxxwjz1Format::TYPE_KEY});
    default:
      return true;
  }
}

bool Dxxwjz1SaxHandler::start_array(const std::size_t elements)
{
  if (mdelegate != nullptr) {
    return delegated_start(mdelegate->start_array(elements));
  }

  if (mstack.empty()) {
    return not_an_object();
  }

  if (in_skipped() || mfield == Field::Skip) {
    push(Context::Skip, "");
    return true;
  }

  if (context() == Context::Values) {
    return fail(where_value() + " is an array, not a float");
  }

  if (context() == Context::Layers) {
    return fail(where() + "[" + std::to_string(mdocument.layers.size()) +
                "] is an array, not a layer object");
  }

  const Field field = take_field();

  switch (field) {
    case Field::Layers:
      push(Context::Layers, Dxxwjz1Format::LAYERS_KEY);
      return true;
    case Field::Values: {
      const WeightsSubLayer sub = mstack.back().sub;

      WeightsArray array;
      array.sub = sub;
      array.kind = mpending_kind;
      layer().arrays.push_back(std::move(array));

      push(Context::Values, WeightsSchema::name(mpending_kind), sub);
      return true;
    }
    case Field::Skip:
      push(Context::Skip, "");
      return true;
    default:
      return wrong_type(field, "array");
  }
}

bool Dxxwjz1SaxHandler::end_array()
{
  if (mdelegate != nullptr) {
    return delegated_end(mdelegate->end_array());
  }

  if (context() == Context::Values) {
    std::vector<float>& values = layer().arrays.back().values;
    values.shrink_to_fit();
  }

  mstack.pop_back();

  return true;
}

bool Dxxwjz1SaxHandler::parse_error(const std::size_t position,
                                    const std::string&,
                                    const nlohmann::detail::exception& ex)
{
  return fail("the JSON syntax error at the byte " + std::to_string(position) +
              " of the text: " + ex.what());
}

bool Dxxwjz1SaxHandler::complete(const bool parsed) const
{
  return parsed && done();
}

const std::string& Dxxwjz1SaxHandler::error() const { return merror; }

const char* Dxxwjz1SaxHandler::format_name() const
{
  return Dxxwjz1Format::NAME;
}

int64_t Dxxwjz1SaxHandler::format_version() const
{
  return Dxxwjz1Format::VERSION;
}

std::vector<std::string> Dxxwjz1SaxHandler::required_keys() const
{
  return {Dxxwjz1Format::FORMAT_KEY, Dxxwjz1Format::VERSION_KEY,
          Dxxwjz1Format::HEADER_KEY, Dxxwjz1Format::LAYERS_KEY};
}

Dxxwjz1SaxHandler::Delegate* Dxxwjz1SaxHandler::extension(const std::string&)
{
  return nullptr;
}

bool Dxxwjz1SaxHandler::extension_end(const std::string&) { return true; }

bool Dxxwjz1SaxHandler::done() const { return mdone && mstack.empty(); }

bool Dxxwjz1SaxHandler::in_weights_array() const
{
  return mdelegate == nullptr && !mstack.empty() &&
         mstack.back().context == Context::Values;
}

bool Dxxwjz1SaxHandler::fail(const std::string& message)
{
  if (merror.empty()) {
    merror = message;
  }

  return false;
}

WeightsDocument& Dxxwjz1SaxHandler::document() { return mdocument; }

WeightsReport& Dxxwjz1SaxHandler::report() { return mreport; }

bool Dxxwjz1SaxHandler::delegated_value(const bool result)
{
  if (!result) {
    return false;
  }

  if (mdelegate_depth > 0U) {
    return true;
  }

  mdelegate = nullptr;

  return extension_end(mdelegate_key);
}

bool Dxxwjz1SaxHandler::delegated_start(const bool result)
{
  ++mdelegate_depth;

  return result;
}

bool Dxxwjz1SaxHandler::delegated_end(const bool result)
{
  --mdelegate_depth;

  return delegated_value(result);
}

bool Dxxwjz1SaxHandler::in_skipped() const
{
  return !mstack.empty() && mstack.back().context == Context::Skip;
}

bool Dxxwjz1SaxHandler::skip_value()
{
  if (in_skipped()) {
    return true;
  }

  if (mfield == Field::Skip) {
    mfield = Field::None;
    return true;
  }

  return false;
}

bool Dxxwjz1SaxHandler::not_an_object()
{
  return fail("the document isn't a JSON object");
}

Dxxwjz1SaxHandler::Context Dxxwjz1SaxHandler::context() const
{
  return mstack.back().context;
}

WeightsLayer& Dxxwjz1SaxHandler::layer() { return mdocument.layers.back(); }

void Dxxwjz1SaxHandler::push(const Context context, const std::string& path,
                             const WeightsSubLayer sub)
{
  Frame frame;
  frame.context = context;
  frame.path = path;
  frame.sub = sub;

  mstack.push_back(std::move(frame));
  mfield = Field::None;
}

Dxxwjz1SaxHandler::Field Dxxwjz1SaxHandler::take_field()
{
  const Field field = mfield;
  mfield = Field::None;
  return field;
}

void Dxxwjz1SaxHandler::restore_field(const Field field) { mfield = field; }

Dxxwjz1SaxHandler::Field Dxxwjz1SaxHandler::field_of(const Frame& frame,
                                                     const std::string& name)
{
  switch (frame.context) {
    case Context::Document:
      if (name == Dxxwjz1Format::FORMAT_KEY) return Field::Format;
      if (name == Dxxwjz1Format::VERSION_KEY) return Field::Version;
      if (name == Dxxwjz1Format::GENERATOR_KEY) return Field::Generator;
      if (name == Dxxwjz1Format::HEADER_KEY) return Field::Header;
      if (name == Dxxwjz1Format::LAYERS_KEY) return Field::Layers;
      if (name == Dxxwjz1Format::TRAILING_KEY) return Field::Trailing;
      return Field::Skip;
    case Context::Header:
      if (name == Dxxwjz1Format::MAJOR_KEY) return Field::Major;
      if (name == Dxxwjz1Format::MINOR_KEY) return Field::Minor;
      if (name == Dxxwjz1Format::REVISION_KEY) return Field::Revision;
      if (name == Dxxwjz1Format::SEEN_KEY) return Field::Seen;
      return Field::Skip;
    case Context::Layer:
      if (name == Dxxwjz1Format::INDEX_KEY) return Field::Index;
      if (name == Dxxwjz1Format::TYPE_KEY) return Field::Type;
      if (name == Dxxwjz1Format::ARRAYS_KEY) {
        mpending_sub = WeightsSubLayer::None;
        return Field::Arrays;
      }
      if (name == Dxxwjz1Format::SUBLAYERS_KEY) return Field::Sublayers;
      return Field::Skip;
    case Context::Sublayers:
      if (!WeightsSchema::parse(name, mpending_sub)) {
        fail(join(name) + " isn't a darknet sub-layer name");
        return Field::None;
      }
      return Field::Sublayer;
    case Context::Sublayer:
      if (name == Dxxwjz1Format::ARRAYS_KEY) {
        mpending_sub = frame.sub;
        return Field::Arrays;
      }
      return Field::Skip;
    case Context::Arrays:
      if (!WeightsSchema::parse(name, mpending_kind)) {
        fail(join(name) + " isn't a darknet weights array name");
        return Field::None;
      }
      // the same array of the sub-layer given twice is the repeated key
      return Field::Values;
    default:
      return Field::Skip;
  }
}

bool Dxxwjz1SaxHandler::version(const number_integer_t value)
{
  if (value != format_version()) {
    return fail("the " + std::string{format_name()} + " version " +
                std::to_string(value) + " isn't supported, the " +
                std::to_string(format_version()) + " one is");
  }

  return true;
}

bool Dxxwjz1SaxHandler::header_value(const Field field,
                                     const number_integer_t value)
{
  if (value < std::numeric_limits<int32_t>::min() ||
      value > std::numeric_limits<int32_t>::max()) {
    return fail(where_of(field) + " " + std::to_string(value) +
                " is out of the 32 bits integers range");
  }

  const auto version = static_cast<int32_t>(value);

  if (field == Field::Major) {
    mdocument.header.major = version;
  } else if (field == Field::Minor) {
    mdocument.header.minor = version;
  } else {
    mdocument.header.revision = version;
  }

  return true;
}

bool Dxxwjz1SaxHandler::layer_index(const number_integer_t value)
{
  if (value < 0 || value > std::numeric_limits<int>::max()) {
    return fail(where_of(Field::Index) + " " + std::to_string(value) +
                " is out of the layer indexes range");
  }

  layer().index = static_cast<int>(value);

  return true;
}

bool Dxxwjz1SaxHandler::append(const float value)
{
  layer().arrays.back().values.push_back(value);
  return appended();
}

bool Dxxwjz1SaxHandler::appended() { return true; }

bool Dxxwjz1SaxHandler::scalar(const char* const type)
{
  if (mstack.empty()) {
    return not_an_object();
  }

  if (skip_value()) {
    return true;
  }

  if (context() == Context::Values) {
    return fail(where_value() + " is the " + type + ", not a float");
  }

  return wrong_type(take_field(), type);
}

bool Dxxwjz1SaxHandler::wrong_type(const Field field, const std::string& type)
{
  if (field == Field::None) {
    return fail(where() + " holds the " + type + " value where the " +
                (context() == Context::Layers ? "layer object" : "key") +
                " is expected");
  }

  return fail(where_of(field) + " is the " + type + ", which is of the " +
              "wrong type");
}

bool Dxxwjz1SaxHandler::require(const Frame& frame,
                                const std::vector<std::string>& keys)
{
  for (const std::string& name : keys) {
    if (std::find(frame.keys.begin(), frame.keys.end(), name) ==
        frame.keys.end()) {
      const std::string path = path_of(mstack.size(), frame.path);

      return fail((path.empty() ? std::string{"the document"} : path) +
                  " has no \"" + name + "\" key");
    }
  }

  return true;
}

std::string Dxxwjz1SaxHandler::path_of(const std::size_t count,
                                       const std::string& part) const
{
  std::string path;

  const auto append_part = [&path](const std::string& text) {
    if (text.empty()) {
      return;
    }

    if (!path.empty() && text.front() != '[') {
      path += ".";
    }

    path += text;
  };

  for (std::size_t index = 0U; index < count; ++index) {
    append_part(mstack[index].path);
  }

  append_part(part);

  return path;
}

std::string Dxxwjz1SaxHandler::where() const
{
  const std::string path = path_of(mstack.size(), "");

  return path.empty() ? std::string{"the document"} : path;
}

std::string Dxxwjz1SaxHandler::join(const std::string& name) const
{
  return path_of(mstack.size(), "\"" + name + "\"");
}

std::string Dxxwjz1SaxHandler::where_of(const Field) const
{
  return join(mkey);
}

std::string Dxxwjz1SaxHandler::where_value() const
{
  return where() + "[" +
         std::to_string(mdocument.layers.back().arrays.back().values.size()) +
         "]";
}

}  // namespace darknet_adaptor::loaders::dxxwjz1::v1
