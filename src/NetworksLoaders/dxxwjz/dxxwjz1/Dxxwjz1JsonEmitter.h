#ifndef DARKNET_CXX_PORT_PROJECT_DXXWJZ1_V1_JSON_EMITTER_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_DXXWJZ1_V1_JSON_EMITTER_CLASS_H

#include <cstddef>
#include <cstdint>
#include <ostream>
#include <string>
#include <vector>

#include "src/NetworksLoaders/weights-model/WeightsDocument.h"
#include "src/NetworksLoaders/weights-model/WeightsSchema.h"
#include "src/darknet.h"

namespace darknet_adaptor::loaders::dxxwjz1::v1
{

/// @brief The array the emitter writes: the one of the document or the
/// network one.
struct Dxxwjz1ArrayView
{
  WeightsSubLayer sub{WeightsSubLayer::None};
  WeightsArrayKind kind{WeightsArrayKind::Weights};
  /// the values, the nullptr for the zeros (the network array, which isn't
  /// allocated)
  const float* data{nullptr};
  std::size_t count{0U};
};

/**
 * @brief Writes the JSON text of the dxxwjz1 document (see the Dxxwjz1Format)
 * into the stream a layer at a time, so neither the document nor its text
 * is kept in the memory: the network arrays go into the stream as they are.
 *
 * The begin(), the layer() of every layer in the ascending index order and
 * the end() make the document; the text keeps a layer on a line and an array
 * on a line. The later versions of the format add their members by the
 * sections().
 */
class Dxxwjz1JsonEmitter
{
 public:
  virtual ~Dxxwjz1JsonEmitter() = default;
  explicit Dxxwjz1JsonEmitter(std::ostream& out);

  Dxxwjz1JsonEmitter(const Dxxwjz1JsonEmitter&) = delete;
  Dxxwjz1JsonEmitter& operator=(const Dxxwjz1JsonEmitter&) = delete;

  /// @brief The format, the version, the generator, the sections(), the
  /// header and the start of the layers, the last two for the weights alone.
  virtual void begin(const WeightsHeader& header, bool weights = true);

  /**
   * @brief One layer with its arrays.
   *
   * @param arrays The arrays in the file order (see the WeightsSchema::rank()),
   * the arrays of the sub-layers one sub-layer after another.
   */
  virtual void layer(int index, LAYER_TYPE type,
                     const std::vector<Dxxwjz1ArrayView>& arrays);

  /// @brief The end of the layers, the trailing bytes and the end of the
  /// document.
  virtual void end(const std::vector<uint8_t>& bytes);

  /// @brief The text generator of the files written.
  static std::string generator();

 protected:
  virtual const char* format_name() const;
  virtual int64_t format_version() const;

  /// @brief The members between the generator and the header, each one
  /// written after the ",\n  " text (none of the first version).
  virtual void sections();

  /// @brief The JSON value of the trailing bytes: the base64 text here.
  virtual std::string trailing(const std::vector<uint8_t>& bytes) const;

  /// @brief The JSON text of the member key: "\"name\": ".
  static std::string key(const char* name);

  /// @brief The JSON text of the header member.
  static std::string header_member(const WeightsHeader& header);

  /// @brief The arrays members of the layer object (see the layer()) and the
  /// end of the object.
  void arrays(const std::vector<Dxxwjz1ArrayView>& arrays);

  /// @brief The JSON array of the floats, the zeros for the nullptr data.
  void values(const float* data, std::size_t count);
  void write(const char* text, std::size_t size);
  void write(const std::string& text);

 private:
  void flush();

  std::ostream& mout;
  std::vector<char> mbuffer;
  std::size_t mused{0U};
  bool mfirst_layer{true};
  bool mweights{true};
};

}  // namespace darknet_adaptor::loaders::dxxwjz1::v1

#endif  // DARKNET_CXX_PORT_PROJECT_DXXWJZ1_V1_JSON_EMITTER_CLASS_H
