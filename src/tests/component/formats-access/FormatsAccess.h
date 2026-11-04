#ifndef DARKNETXX_PROJECT_TESTS_FORMATS_ACCESS_H
#define DARKNETXX_PROJECT_TESTS_FORMATS_ACCESS_H

#include <zlib.h>

#include <cstdint>
#include <cstring>
#include <fstream>
#include <istream>
#include <memory>
#include <ostream>
#include <string>
#include <utility>
#include <vector>

#include "src/NetworksLoaders/DataDriversAliases.h"
#include "src/NetworksLoaders/network-weights/NetworkWeights.h"
#include "src/NetworksLoaders/network-weights/NetworkWeightsConverter.h"
#include "src/darknet-adaptor/adaptors/ANetworkAdaptor/ANetworkAdaptor.h"
#include "src/darknet.h"

/**
 * @file
 *
 * The network weights files formats of the darknetxx-datadrivers library as
 * the tests of the darknet networks name them.
 *
 * The formats left the project for the library, which exports the registry of
 * them alone (see the dxxdd::WeightsFileFormats): the readers and the writers
 * of every format are the classes the visibility of the library keeps inside
 * of it. The tests of the darknet networks of the formats read and write the
 * files through the darknet networks (the NetworkWeights view of the library
 * over them), so the classes below give them the names and the calls of the
 * darknet network the tests were written against: the reader and the writer
 * of the format name over the registry one, the darknet network handed over as
 * the view of it. Nothing of the formats is implemented here, every call is
 * the one of the library.
 */

// The comparisons of the layer types of the library with the darknet ones,
// the very same values (see the NetworkWeights). In the global namespace, the
// one of the LAYER_TYPE, so the argument dependent lookup of the gtest
// comparisons finds them.
inline bool operator==(const dxxdd::LayerType lhs, const LAYER_TYPE rhs)
{
  return static_cast<int>(lhs) == static_cast<int>(rhs);
}

inline bool operator==(const LAYER_TYPE lhs, const dxxdd::LayerType rhs)
{
  return rhs == lhs;
}

inline bool operator!=(const dxxdd::LayerType lhs, const LAYER_TYPE rhs)
{
  return !(lhs == rhs);
}

inline bool operator!=(const LAYER_TYPE lhs, const dxxdd::LayerType rhs)
{
  return !(rhs == lhs);
}

namespace DarknetxxDataDrivers012
{

/// @brief The gtest printer of the library layer types: the cfg section name.
inline void PrintTo(const LayerType type, std::ostream* out)
{
  *out << WeightsSchema::name(type);
}

}  // namespace DarknetxxDataDrivers012

namespace darknet_adaptor::loaders
{

// the library types the tests of the darknet networks name
using dxxdd::CfgSection;
using dxxdd::CfgSections;
using dxxdd::IWeightsFileFormat;
using dxxdd::IWeightsFileFormatPtr;
using dxxdd::NetworkDescription;
using dxxdd::NetworkFiles;
using dxxdd::NetworkSource;
using dxxdd::TrainingRecord;
using dxxdd::WeightsArray;
using dxxdd::WeightsArrayKind;
using dxxdd::WeightsConversion;
using dxxdd::WeightsDocument;
using dxxdd::WeightsFileFormats;
using dxxdd::WeightsHeader;
using dxxdd::WeightsLayer;
using dxxdd::WeightsLayoutEntries;
using dxxdd::WeightsLayoutEntry;
using dxxdd::WeightsLayoutOptions;
using dxxdd::WeightsReport;
using dxxdd::WeightsSubLayer;
using dxxdd::Yolo4Annotations;
using dxxdd::Yolo4Box;
using dxxdd::Yolo4Image;

/// @brief The converter of the darknet networks: the converter of the
/// library, which reads the original weights files by the darknet network of
/// their cfg the very way the darknetxx binary does.
using WeightsConverter = NetworkWeightsConverter;

/// @brief The library layer type of the darknet one.
inline dxxdd::LayerType library_type(const LAYER_TYPE type)
{
  return NetworkWeights::type_of(type);
}

/**
 * @brief The schema of the library of the darknet layer types as well: the
 * names, the ranks and the arrays of the LAYER_TYPE values the tests of the
 * darknet networks give it.
 */
struct WeightsSchema : dxxdd::WeightsSchema
{
  using dxxdd::WeightsSchema::arrays;
  using dxxdd::WeightsSchema::has_weights;
  using dxxdd::WeightsSchema::name;
  using dxxdd::WeightsSchema::parse;
  using dxxdd::WeightsSchema::rank;
  using dxxdd::WeightsSchema::sublayer_type;
  using dxxdd::WeightsSchema::sublayers;

  static const std::string& name(const LAYER_TYPE type)
  {
    return dxxdd::WeightsSchema::name(NetworkWeights::type_of(type));
  }

  static bool parse(const std::string& text, LAYER_TYPE& type)
  {
    dxxdd::LayerType parsed = dxxdd::LayerType::BLANK;

    if (!dxxdd::WeightsSchema::parse(text, parsed)) {
      return false;
    }

    type = NetworkWeights::type_of(parsed);

    return true;
  }

  static bool has_weights(const LAYER_TYPE type)
  {
    return dxxdd::WeightsSchema::has_weights(NetworkWeights::type_of(type));
  }

  static const std::vector<dxxdd::WeightsSubLayer>& sublayers(
      const LAYER_TYPE type)
  {
    return dxxdd::WeightsSchema::sublayers(NetworkWeights::type_of(type));
  }

  static LAYER_TYPE sublayer_type(const LAYER_TYPE type)
  {
    return NetworkWeights::type_of(
        dxxdd::WeightsSchema::sublayer_type(NetworkWeights::type_of(type)));
  }

  static const std::vector<dxxdd::WeightsArrayKind>& arrays(
      const LAYER_TYPE type)
  {
    return dxxdd::WeightsSchema::arrays(NetworkWeights::type_of(type));
  }

  static int rank(const LAYER_TYPE type, const dxxdd::WeightsSubLayer sub,
                  const dxxdd::WeightsArrayKind kind)
  {
    return dxxdd::WeightsSchema::rank(NetworkWeights::type_of(type), sub, kind);
  }
};

/// @brief The registry format of the name: of every format, the YOLO4 folder
/// one included.
inline IWeightsFileFormatPtr format_named(const std::string& name)
{
  return WeightsFileFormats::named(name, WeightsFileFormats::convertible());
}

/// @brief The layouts of the darknet networks of the NetworkWeights view and
/// the library helpers of them.
struct WeightsLayout
{
  static WeightsLayoutEntries of(
      const network& net,
      const WeightsLayoutOptions& options = WeightsLayoutOptions{})
  {
    return NetworkWeights::layout_of(net, options);
  }

  static uint64_t floats(const WeightsLayoutEntries& layout)
  {
    return dxxdd::WeightsLayout::floats(layout);
  }

  static void transpose_matrix(float* a, const int rows, const int cols)
  {
    dxxdd::WeightsLayout::transpose_matrix(a, rows, cols);
  }
};

/**
 * @brief The reader of the format of the library registry, which loads the
 * files into the darknet networks and reads them by the structure of the
 * darknet networks.
 */
class NetworkFileReader
{
 public:
  explicit NetworkFileReader(const std::string& format)
      : mreader{format_named(format)->create_reader()}
  {
  }

  virtual ~NetworkFileReader() = default;

  bool load(network& net, const std::string& filename, const int cutoff)
  {
    return mreader->load(NetworkWeights{net}, filename, cutoff);
  }

  bool load(network& net, const std::string& filename)
  {
    return load(net, filename, net.n);
  }

  bool load(adaptors::ANetworkAdaptor& net, const std::string& filename)
  {
    return load(*net.get(), filename);
  }

  bool load(network& net, const std::string& filename, const int cutoff,
            WeightsDocument& rest)
  {
    return mreader->load(NetworkWeights{net}, filename, cutoff, rest);
  }

  bool peek(const std::string& filename, WeightsDocument& rest, bool& weights)
  {
    return mreader->peek(filename, rest, weights);
  }

  bool read(const std::string& filename, const network* net,
            WeightsDocument& document)
  {
    if (net == nullptr) {
      return mreader->read(filename, nullptr, document);
    }

    const NetworkWeights view{*net};

    return mreader->read(filename, &view, document);
  }

  /// @brief The reader of the library itself.
  const dxxdd::IWeightsReaderPtr& library() const { return mreader; }

 private:
  dxxdd::IWeightsReaderPtr mreader;
};

/**
 * @brief The writer of the format of the library registry, which saves the
 * files of the darknet networks.
 */
class NetworkFileWriter
{
 public:
  explicit NetworkFileWriter(const std::string& format)
      : mwriter{format_named(format)->create_writer()}
  {
  }

  virtual ~NetworkFileWriter() = default;

  bool save(network& net, const std::string& filename, const int cutoff,
            const bool save_ema)
  {
    return mwriter->save(NetworkWeights{net}, filename, cutoff, save_ema);
  }

  bool save(network& net, const std::string& filename)
  {
    return save(net, filename, net.n, false);
  }

  bool save(adaptors::ANetworkAdaptor& net, const std::string& filename)
  {
    return save(*net.get(), filename);
  }

  bool save(network& net, const std::string& filename, const int cutoff,
            const bool save_ema, const WeightsDocument& rest)
  {
    return mwriter->save(NetworkWeights{net}, filename, cutoff, save_ema, rest);
  }

  bool write(const WeightsDocument& document, const std::string& filename)
  {
    return mwriter->write(document, filename);
  }

  /// @brief The writer of the library itself.
  const dxxdd::IWeightsWriterPtr& library() const { return mwriter; }

 private:
  dxxdd::IWeightsWriterPtr mwriter;
};

/**
 * @brief The reader and the writer of the format name the tests of the
 * darknet networks call by the old class names.
 */
template <const char* const* FORMAT>
class FormatReader : public NetworkFileReader
{
 public:
  FormatReader() : NetworkFileReader{*FORMAT} {}

  static std::shared_ptr<FormatReader> create()
  {
    return std::make_shared<FormatReader>();
  }
};

template <const char* const* FORMAT>
class FormatWriter : public NetworkFileWriter
{
 public:
  FormatWriter() : NetworkFileWriter{*FORMAT} {}

  static std::shared_ptr<FormatWriter> create()
  {
    return std::make_shared<FormatWriter>();
  }
};

namespace formats_access
{

inline constexpr const char* WEIGHTS = "weights";
inline constexpr const char* DXXWJZ1 = "dxxwjz1";
inline constexpr const char* DXXWJZ2 = "dxxwjz2";
inline constexpr const char* DXXWJZ2P = "dxxwjz2p";
inline constexpr const char* DXXWJZ3 = "dxxwjz3";
inline constexpr const char* DXXWJZ3P = "dxxwjz3p";
inline constexpr const char* DXXWJZ4 = "dxxwjz4";
inline constexpr const char* DXXWJZ4P = "dxxwjz4p";
inline constexpr const char* YOLO4 = "yolo4";

}  // namespace formats_access

/// @brief The original darknet weights files of the darknet networks.
class OrigWeightsReader : public FormatReader<&formats_access::WEIGHTS>
{
 public:
  static std::shared_ptr<OrigWeightsReader> create()
  {
    return std::make_shared<OrigWeightsReader>();
  }

  static void transpose_matrix(float* a, const int rows, const int cols)
  {
    dxxdd::WeightsLayout::transpose_matrix(a, rows, cols);
  }
};

class OrigWeightsWriter : public FormatWriter<&formats_access::WEIGHTS>
{
 public:
  static std::shared_ptr<OrigWeightsWriter> create()
  {
    return std::make_shared<OrigWeightsWriter>();
  }
};

namespace dxxwjz1::v1
{
using Dxxwjz1Reader = FormatReader<&formats_access::DXXWJZ1>;
using Dxxwjz1Writer = FormatWriter<&formats_access::DXXWJZ1>;
}  // namespace dxxwjz1::v1

namespace dxxwjz2::v2
{
using Dxxwjz2Reader = FormatReader<&formats_access::DXXWJZ2>;
using Dxxwjz2Writer = FormatWriter<&formats_access::DXXWJZ2>;
}  // namespace dxxwjz2::v2

namespace dxxwjz2p::v1
{
using Dxxwjz2pReader = FormatReader<&formats_access::DXXWJZ2P>;
using Dxxwjz2pWriter = FormatWriter<&formats_access::DXXWJZ2P>;
}  // namespace dxxwjz2p::v1

namespace dxxwjz3::v3
{
using Dxxwjz3Reader = FormatReader<&formats_access::DXXWJZ3>;
using Dxxwjz3Writer = FormatWriter<&formats_access::DXXWJZ3>;
}  // namespace dxxwjz3::v3

namespace dxxwjz3p::v1
{
using Dxxwjz3pReader = FormatReader<&formats_access::DXXWJZ3P>;
using Dxxwjz3pWriter = FormatWriter<&formats_access::DXXWJZ3P>;
}  // namespace dxxwjz3p::v1

namespace dxxwjz4::v4
{
using Dxxwjz4Reader = FormatReader<&formats_access::DXXWJZ4>;
using Dxxwjz4Writer = FormatWriter<&formats_access::DXXWJZ4>;
}  // namespace dxxwjz4::v4

namespace dxxwjz4p::v1
{
using Dxxwjz4pReader = FormatReader<&formats_access::DXXWJZ4P>;
using Dxxwjz4pWriter = FormatWriter<&formats_access::DXXWJZ4P>;
}  // namespace dxxwjz4p::v1

namespace yolo4
{
using Yolo4FolderReader = FormatReader<&formats_access::YOLO4>;
using Yolo4FolderWriter = FormatWriter<&formats_access::YOLO4>;
}  // namespace yolo4

/**
 * @brief The gzip files of the dxxwjz documents the tests read and write
 * themselves, through the zlib (the library keeps it's own gzip streams
 * inside of it).
 */
namespace gzip_files
{

/// @brief Tells the file starts with the gzip magic bytes.
inline bool is_gzip_file(const std::string& path)
{
  std::ifstream file{path, std::ios::binary};
  unsigned char magic[2] = {0U, 0U};

  file.read(reinterpret_cast<char*>(magic), sizeof(magic));

  return file.gcount() == 2 && magic[0] == 0x1FU && magic[1] == 0x8BU;
}

/// @brief The uncompressed content of the gzip file, false for the file of
/// no gzip stream (or unable to be read).
inline bool read(const std::string& path, std::string& content)
{
  gzFile file = gzopen(path.c_str(), "rb");

  if (file == nullptr) {
    return false;
  }

  content.clear();

  char chunk[65536];
  int count = 0;

  while ((count = gzread(file, chunk, sizeof(chunk))) > 0) {
    content.append(chunk, static_cast<std::size_t>(count));
  }

  int error = Z_OK;
  gzerror(file, &error);
  const bool complete = count == 0 && (error == Z_OK || error == Z_STREAM_END);

  gzclose(file);

  return complete;
}

/// @brief Writes the content into the gzip file of the compression level.
inline bool write(const std::string& path, const std::string& content,
                  const int level = Z_DEFAULT_COMPRESSION)
{
  const std::string mode = "wb" + std::to_string(level < 0 ? 6 : level);
  gzFile file = gzopen(path.c_str(), mode.c_str());

  if (file == nullptr) {
    return false;
  }

  const bool written =
      content.empty() ||
      gzwrite(file, content.data(), static_cast<unsigned>(content.size())) ==
          static_cast<int>(content.size());

  return gzclose(file) == Z_OK && written;
}

/// @brief The uncompressed content of the gzip stream in memory, empty for
/// the bytes of no gzip stream.
inline std::string uncompress(const std::vector<uint8_t>& bytes)
{
  z_stream stream{};

  if (inflateInit2(&stream, 15 + 16) != Z_OK) {
    return {};
  }

  std::string content;
  std::vector<char> chunk(65536U);

  stream.next_in = const_cast<Bytef*>(bytes.data());
  stream.avail_in = static_cast<uInt>(bytes.size());

  int status = Z_OK;

  do {
    stream.next_out = reinterpret_cast<Bytef*>(chunk.data());
    stream.avail_out = static_cast<uInt>(chunk.size());
    status = inflate(&stream, Z_NO_FLUSH);

    if (status != Z_OK && status != Z_STREAM_END) {
      inflateEnd(&stream);
      return {};
    }

    content.append(chunk.data(), chunk.size() - stream.avail_out);
  } while (status != Z_STREAM_END);

  inflateEnd(&stream);

  return content;
}

}  // namespace gzip_files

}  // namespace darknet_adaptor::loaders

#endif  // DARKNETXX_PROJECT_TESTS_FORMATS_ACCESS_H
