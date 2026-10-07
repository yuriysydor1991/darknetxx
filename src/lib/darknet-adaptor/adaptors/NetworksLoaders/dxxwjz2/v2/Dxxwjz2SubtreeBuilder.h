#ifndef DARKNET_CXX_PORT_PROJECT_DXXWJZ2_V2_SUBTREE_BUILDER_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_DXXWJZ2_V2_SUBTREE_BUILDER_CLASS_H

#include <cstddef>
#include <functional>
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

namespace darknet_adaptor::loaders::dxxwjz2::v2
{

/**
 * @brief Builds the JSON tree of a value of the document of its SAX events:
 * the members the dxxwjz2 files add to the dxxwjz1 ones, which are small
 * beside the weights. The fractions are the floats their texts are the
 * decimals of exactly (see the Dxxwjz1FloatCodec), the repeated keys fail
 * the build.
 */
class Dxxwjz2SubtreeBuilder : public nlohmann::json_sax<nlohmann::json>
{
 public:
  using json = nlohmann::json;
  /// @brief Takes the error message, returns false.
  using Failure = std::function<bool(const std::string&)>;

  virtual ~Dxxwjz2SubtreeBuilder() = default;
  explicit Dxxwjz2SubtreeBuilder(Failure nfail);

  /// @brief Starts the tree of the value of the document key.
  void reset(const std::string& name);

  const json& tree() const;

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
  /// @brief Puts the value at the current position, the container ones
  /// become the current ones.
  bool add(json value, bool container);

  /// @brief The JSON path of the value at the current position, the last
  /// key quoted for the messages.
  std::string path(bool quoted = true) const;

  Failure fail;
  json mtree;
  /// the open containers with their paths
  std::vector<json*> mstack;
  std::vector<std::string> mpaths;
  std::string mkey;
  const char mpoint;
};

}  // namespace darknet_adaptor::loaders::dxxwjz2::v2

#endif  // DARKNET_CXX_PORT_PROJECT_DXXWJZ2_V2_SUBTREE_BUILDER_CLASS_H
