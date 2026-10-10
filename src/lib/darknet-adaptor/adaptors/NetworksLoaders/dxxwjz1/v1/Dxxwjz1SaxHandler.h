#ifndef DARKNET_CXX_PORT_PROJECT_DXXWJZ1_V1_SAX_HANDLER_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_DXXWJZ1_V1_SAX_HANDLER_CLASS_H

#include <cstddef>
#include <cstdint>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz1/v1/Dxxwjz1FloatCodec.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/weights-model/WeightsDocument.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/weights-model/WeightsReport.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/weights-model/WeightsSchema.h"

namespace darknet_adaptor::loaders::dxxwjz1::v1
{

/**
 * @brief Builds the document of the SAX events of the dxxwjz1 JSON text (see
 * the Dxxwjz1Parser). Every callback returns false on the first error, which
 * stops the parse.
 *
 * The later versions of the format check their own format name, version and
 * required keys and hand the values of their own document keys to the
 * handlers of theirs (see the extension()).
 */
class Dxxwjz1SaxHandler : public nlohmann::json_sax<nlohmann::json>
{
 public:
  using json = nlohmann::json;
  using Delegate = nlohmann::json_sax<json>;

  virtual ~Dxxwjz1SaxHandler() = default;
  Dxxwjz1SaxHandler(WeightsDocument& document, WeightsReport& report);

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

  /// @brief Tells the parse result made the whole document.
  virtual bool complete(bool parsed) const;

  const std::string& error() const;

 protected:
  virtual const char* format_name() const;
  virtual int64_t format_version() const;

  /// @brief The keys every document has.
  virtual std::vector<std::string> required_keys() const;

  /// @brief The handler of the value of a document key of a later version,
  /// which may take over the keys of the earlier ones (the trailing one),
  /// nullptr for the rest: the unknown keys are skipped with a warning.
  virtual Delegate* extension(const std::string& name);

  /// @brief The value of the extension() key is complete, false fails the
  /// parse.
  virtual bool extension_end(const std::string& name);

  /// @brief A float of a weights array is appended, false stops the parse
  /// (the peek of the later versions), true here.
  virtual bool appended();

  /// @brief True once the whole document is parsed.
  bool done() const;

  bool in_weights_array() const;

  bool fail(const std::string& message);

  WeightsDocument& document();
  WeightsReport& report();

 private:
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

  /// @brief Hands the extension() value events over, the result of the
  /// delegate given.
  bool delegated_value(bool result);
  bool delegated_start(bool result);
  bool delegated_end(bool result);

  /// @brief True inside of the object or the array of an unknown key.
  bool in_skipped() const;

  /// @brief Consumes the scalar value of an unknown key, true for such one.
  bool skip_value();

  bool not_an_object();

  Context context() const;

  WeightsLayer& layer();

  void push(Context context, const std::string& path,
            WeightsSubLayer sub = WeightsSubLayer::None);

  /// @brief The field the value is of, which the value consumes.
  Field take_field();

  void restore_field(Field field);

  /// @brief The field of the key of the object, Field::None on the error.
  Field field_of(const Frame& frame, const std::string& name);

  bool version(number_integer_t value);
  bool header_value(Field field, number_integer_t value);
  bool layer_index(number_integer_t value);
  bool append(float value);
  bool scalar(const char* type);
  bool wrong_type(Field field, const std::string& type);
  bool require(const Frame& frame, const std::vector<std::string>& keys);

  /// @brief The JSON path of the frames up to the count and the part.
  std::string path_of(std::size_t count, const std::string& part) const;

  std::string where() const;
  std::string join(const std::string& name) const;
  std::string where_of(Field field) const;
  std::string where_value() const;

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
  /// the extension() handler of the value being parsed, its key and depth
  Delegate* mdelegate{nullptr};
  std::string mdelegate_key;
  std::size_t mdelegate_depth{0U};
};

}  // namespace darknet_adaptor::loaders::dxxwjz1::v1

#endif  // DARKNET_CXX_PORT_PROJECT_DXXWJZ1_V1_SAX_HANDLER_CLASS_H
