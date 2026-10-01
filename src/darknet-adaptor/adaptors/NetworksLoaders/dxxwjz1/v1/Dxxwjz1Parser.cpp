#include "src/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz1/v1/Dxxwjz1Parser.h"

#if defined(__unix__)
// the newlocale() and the uselocale() of the POSIX 2008
#include <locale.h>
#endif

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <istream>
#include <limits>
#include <memory>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

#include "src/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz1/v1/Dxxwjz1Base64.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz1/v1/Dxxwjz1FloatCodec.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz1/v1/Dxxwjz1Format.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/weights-model/WeightsDocument.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/weights-model/WeightsReport.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/weights-model/WeightsSchema.h"
#include "src/zlib/ZlibController.h"

namespace darknet_adaptor::loaders::dxxwjz1::v1
{

namespace
{

using json = nlohmann::json;

/**
 * @brief Runs the thread in the "C" numeric locale for the life of the
 * object.
 *
 * The JSON lexer puts the first byte of the decimal point of the current
 * locale in place of the '.' of the number texts and converts them by the
 * strtod() of the locale, which the multibyte decimal points break (the
 * U+066B one of the ps_AF locale, for example): the strtod() stops at the
 * byte after the first one, which the lexer asserts against. The locale of
 * the thread alone is changed, so the global one of the application and the
 * other threads are kept. Where the thread locales aren't there, the
 * decimal point the lexer puts in the number texts is replaced back (see the
 * Dxxwjz1FloatCodec::decode_number()).
 */
class ClassicNumericLocale
{
 public:
  ClassicNumericLocale()
  {
#if defined(__unix__)
    locale_t base = duplocale(uselocale(locale_t{}));

    if (base == locale_t{}) {
      return;
    }

    mlocale = newlocale(LC_NUMERIC_MASK, "C", base);

    if (mlocale == locale_t{}) {
      freelocale(base);
      return;
    }

    mprevious = uselocale(mlocale);
#endif
  }

  ~ClassicNumericLocale()
  {
#if defined(__unix__)
    if (mlocale != locale_t{}) {
      uselocale(mprevious);
      freelocale(mlocale);
    }
#endif
  }

  ClassicNumericLocale(const ClassicNumericLocale&) = delete;
  ClassicNumericLocale& operator=(const ClassicNumericLocale&) = delete;

 private:
#if defined(__unix__)
  locale_t mlocale{};
  locale_t mprevious{};
#endif
};

/// @brief The JSON value the parser is inside of.
enum class Context : uint8_t
{
  Document,
  Header,
  Layers,
  Layer,
  Sublayers,
  Sublayer,
  Arrays,
  Values,
  /// the value of an unknown key
  Skip
};

/// @brief The value the parser expects next inside of an object.
enum class Field : uint8_t
{
  None,
  Format,
  Version,
  Generator,
  Header,
  Layers,
  Trailing,
  Major,
  Minor,
  Revision,
  Seen,
  Index,
  Type,
  Arrays,
  Sublayers,
  Sublayer,
  Values,
  Skip
};

struct Frame
{
  Context context{Context::Document};
  /// the JSON path part of the value: the key or the position in the array
  std::string path;
  /// the keys of the object given so far
  std::vector<std::string> keys;
  /// the sub-layer of the arrays
  WeightsSubLayer sub{WeightsSubLayer::None};
};

/**
 * @brief Builds the document of the SAX events. Every callback returns false
 * on the first error, which stops the parse.
 */
class Dxxwjz1SaxHandler : public nlohmann::json_sax<json>
{
 public:
  Dxxwjz1SaxHandler(WeightsDocument& document, WeightsReport& report)
      : mdocument{document}, mreport{report}
  {
  }

  bool null() override { return scalar("null"); }

  bool boolean(bool) override { return scalar("boolean"); }

  bool number_integer(const number_integer_t value) override
  {
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

  bool number_unsigned(const number_unsigned_t value) override
  {
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

  bool number_float(number_float_t, const string_t& text) override
  {
    if (mstack.empty()) {
      return not_an_object();
    }

    if (skip_value()) {
      return true;
    }

    if (context() == Context::Values) {
      float value = 0.f;

      if (!Dxxwjz1FloatCodec::decode_number(text, value, mpoint)) {
        return fail(where_value() + " " + text +
                    " is out of the float range (write the non finite floats "
                    "as their 0x bit pattern strings)");
      }

      return append(value);
    }

    return wrong_type(take_field(), "fraction");
  }

  bool string(string_t& text) override
  {
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
        if (text != Dxxwjz1Format::NAME) {
          return fail("the format \"" + text + "\" isn't the " +
                      Dxxwjz1Format::NAME + " one");
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

  bool binary(binary_t&) override { return scalar("binary"); }

  bool start_object(std::size_t) override
  {
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
        push(Context::Sublayer, WeightsSchema::name(mpending_sub),
             mpending_sub);
        return true;
      case Field::Skip:
        push(Context::Skip, "");
        return true;
      default:
        return wrong_type(field, "object");
    }
  }

  bool key(string_t& name) override
  {
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

    if (mfield == Field::Skip) {
      mreport.warning("the unknown key " + join(name) + " is skipped");
    }

    return true;
  }

  bool end_object() override
  {
    const Frame frame = mstack.back();

    mstack.pop_back();

    switch (frame.context) {
      case Context::Document:
        mdone = true;
        return require(frame,
                       {Dxxwjz1Format::FORMAT_KEY, Dxxwjz1Format::VERSION_KEY,
                        Dxxwjz1Format::HEADER_KEY, Dxxwjz1Format::LAYERS_KEY});
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

  bool start_array(std::size_t) override
  {
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

  bool end_array() override
  {
    if (context() == Context::Values) {
      std::vector<float>& values = layer().arrays.back().values;
      values.shrink_to_fit();
    }

    mstack.pop_back();

    return true;
  }

  bool parse_error(std::size_t position, const std::string&,
                   const nlohmann::detail::exception& ex) override
  {
    return fail("the JSON syntax error at the byte " +
                std::to_string(position) + " of the text: " + ex.what());
  }

  /// @brief True once the whole document is parsed.
  bool done() const { return mdone && mstack.empty(); }

  const std::string& error() const { return merror; }

 private:
  /// @brief True inside of the object or the array of an unknown key.
  bool in_skipped() const
  {
    return !mstack.empty() && mstack.back().context == Context::Skip;
  }

  /// @brief Consumes the scalar value of an unknown key, true for such one.
  bool skip_value()
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

  bool not_an_object() { return fail("the document isn't a JSON object"); }

  Context context() const { return mstack.back().context; }

  WeightsLayer& layer() { return mdocument.layers.back(); }

  void push(const Context context, const std::string& path,
            const WeightsSubLayer sub = WeightsSubLayer::None)
  {
    Frame frame;
    frame.context = context;
    frame.path = path;
    frame.sub = sub;

    mstack.push_back(std::move(frame));
    mfield = Field::None;
  }

  /// @brief The field the value is of, which the value consumes.
  Field take_field()
  {
    const Field field = mfield;
    mfield = Field::None;
    return field;
  }

  void restore_field(const Field field) { mfield = field; }

  /// @brief The field of the key of the object, Field::None on the error.
  Field field_of(const Frame& frame, const std::string& name)
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

  bool version(const number_integer_t value)
  {
    if (value != Dxxwjz1Format::VERSION) {
      return fail("the " + std::string{Dxxwjz1Format::NAME} + " version " +
                  std::to_string(value) + " isn't supported, the " +
                  std::to_string(Dxxwjz1Format::VERSION) + " one is");
    }

    return true;
  }

  bool header_value(const Field field, const number_integer_t value)
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

  bool layer_index(const number_integer_t value)
  {
    if (value < 0 || value > std::numeric_limits<int>::max()) {
      return fail(where_of(Field::Index) + " " + std::to_string(value) +
                  " is out of the layer indexes range");
    }

    layer().index = static_cast<int>(value);

    return true;
  }

  bool append(const float value)
  {
    layer().arrays.back().values.push_back(value);
    return true;
  }

  bool scalar(const char* const type)
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

  bool wrong_type(const Field field, const std::string& type)
  {
    if (field == Field::None) {
      return fail(where() + " holds the " + type + " value where the " +
                  (context() == Context::Layers ? "layer object" : "key") +
                  " is expected");
    }

    return fail(where_of(field) + " is the " + type + ", which is of the " +
                "wrong type");
  }

  bool require(const Frame& frame, const std::vector<std::string>& keys)
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

  /// @brief The JSON path of the frames up to the count and the part.
  std::string path_of(const std::size_t count, const std::string& part) const
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

  std::string where() const
  {
    const std::string path = path_of(mstack.size(), "");

    return path.empty() ? std::string{"the document"} : path;
  }

  std::string join(const std::string& name) const
  {
    return path_of(mstack.size(), "\"" + name + "\"");
  }

  std::string where_of(const Field) const { return join(mkey); }

  std::string where_value() const
  {
    return where() + "[" +
           std::to_string(mdocument.layers.back().arrays.back().values.size()) +
           "]";
  }

  bool fail(const std::string& message)
  {
    if (merror.empty()) {
      merror = message;
    }

    return false;
  }

  WeightsDocument& mdocument;
  WeightsReport& mreport;
  /// the decimal point the JSON lexer keeps in the number texts
  const char mpoint{Dxxwjz1FloatCodec::decimal_point()};
  std::vector<Frame> mstack;
  Field mfield{Field::None};
  std::string mkey;
  WeightsSubLayer mpending_sub{WeightsSubLayer::None};
  WeightsArrayKind mpending_kind{WeightsArrayKind::Weights};
  bool mdone{false};
  std::string merror;
};

}  // namespace

bool Dxxwjz1Parser::parse(std::istream& json, WeightsDocument& document,
                          WeightsReport& report)
{
  document = WeightsDocument{};

  // the number texts of the JSON lexer of any locale of the application
  const ClassicNumericLocale classic;

  Dxxwjz1SaxHandler handler{document, report};

  const bool parsed =
      json::sax_parse(json, &handler, json::input_format_t::json, true, false);

  if (!parsed || !handler.done()) {
    report.error(handler.error().empty() ? "the JSON text isn't complete"
                                         : handler.error());
    document = WeightsDocument{};
    return false;
  }

  document.normalize();

  return true;
}

bool Dxxwjz1Parser::parse_file(const std::string& filename,
                               WeightsDocument& document, WeightsReport& report)
{
  std::unique_ptr<zlibi::GzipIStream> file =
      zlibi::ZlibController::create()->create_file_reader(filename);

  if (file == nullptr) {
    report.error("the file can't be opened");
    return false;
  }

  // the plain file of anything but the JSON text (the original weights file,
  // for example) fails the parse at its first byte
  if (file->direct()) {
    *file >> std::ws;

    if (file->peek() != '{') {
      report.error(
          "the file is neither the gzip compressed nor the plain "
          "JSON text (the original weights file?)");
      return false;
    }
  }

  const bool parsed = parse(*file, document, report);

  // the gzip failure is the reason of the JSON one it leads to
  if (file->failed()) {
    report = WeightsReport{};
    report.error("the gzip data is corrupted or truncated: " + file->error());
    document = WeightsDocument{};
    return false;
  }

  if (file->direct()) {
    report.warning(
        "the file isn't gzip compressed, it's read as the plain "
        "JSON text");
  }

  return parsed;
}

}  // namespace darknet_adaptor::loaders::dxxwjz1::v1
