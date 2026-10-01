#ifndef DARKNET_CXX_PORT_PROJECT_WEIGHTS_DOCUMENT_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_WEIGHTS_DOCUMENT_CLASS_H

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

#include "src/darknet-adaptor/adaptors/NetworksLoaders/weights-model/WeightsSchema.h"
#include "src/darknet.h"

namespace darknet_adaptor::loaders
{

/**
 * @brief The weights file header: the version triple and the count of the
 * images the network has seen.
 *
 * The version tells the way the loaders read the file, the way the original
 * load_weights_upto() did: the (major * 10 + minor) >= 2 versions keep the 64
 * bits seen and the older ones the 32 bits one, the loaders transpose the
 * connected weights of the major or minor > 1000 versions.
 */
struct WeightsHeader
{
  int32_t major{0};
  int32_t minor{0};
  int32_t revision{0};
  uint64_t seen{0U};

  /// @brief The header darknet writes: the current version and the seen.
  static WeightsHeader current(uint64_t seen);

  /// @brief The (major * 10 + minor) >= 2 versions keep the 64 bits seen.
  bool seen64() const;

  /// @brief The loaders transpose the connected weights of the major or
  /// minor > 1000 versions.
  bool transposed() const;

  /// @brief The size of the header in the original weights file: 20 bytes
  /// of the 64 bits seen, 16 bytes of the 32 bits one.
  std::size_t size() const;

  /// @brief The "major.minor.revision" version text.
  std::string version() const;

  bool operator==(const WeightsHeader& other) const;
  bool operator!=(const WeightsHeader& other) const;
};

/// @brief One float array of the layer or of its sub-layer.
struct WeightsArray
{
  WeightsSubLayer sub{WeightsSubLayer::None};
  WeightsArrayKind kind{WeightsArrayKind::Weights};
  std::vector<float> values;
};

/// @brief One layer of the network with the arrays of it.
struct WeightsLayer
{
  int index{0};
  LAYER_TYPE type{BLANK};
  /// the arrays in the file order (see the WeightsSchema::rank())
  std::vector<WeightsArray> arrays;

  /// @brief The array of the layer, nullptr for none.
  const WeightsArray* find(WeightsSubLayer sub, WeightsArrayKind kind) const;
};

/**
 * @brief The format neutral weights file content: the header, the layers of
 * the network with their arrays and the bytes the file keeps after the
 * arrays.
 *
 * The readers of the weights formats read the files into the document and the
 * writers write the document, which converts the files between the formats:
 * the arrays in the order of the layers and of the WeightsSchema::rank() are
 * the payload of the original darknet weights file, the trailing bytes follow
 * them. A layer without some or all of its arrays is the layer of the
 * truncated file (or the layer, which keeps no weights), an array shorter
 * than the network one is the truncated one.
 */
struct WeightsDocument
{
  WeightsHeader header;
  /// the layers in the ascending index order
  std::vector<WeightsLayer> layers;
  /// the bytes after the arrays: the part of the float of the truncated
  /// file, the data beyond the network ones
  std::vector<uint8_t> trailing;

  /// @brief The layer of the index, nullptr for none.
  const WeightsLayer* find(int index) const;

  /// @brief The count of the floats of every array.
  uint64_t floats() const;

  /// @brief The size of the original weights file of the document.
  uint64_t file_size() const;

  /// @brief Sorts the layers by the index and their arrays by the file order.
  void normalize();
};

}  // namespace darknet_adaptor::loaders

#endif  // DARKNET_CXX_PORT_PROJECT_WEIGHTS_DOCUMENT_CLASS_H
