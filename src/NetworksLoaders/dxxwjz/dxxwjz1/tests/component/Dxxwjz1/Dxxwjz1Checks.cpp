#include "Dxxwjz1Checks.h"

#include <zlib.h>

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <limits>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include "src/NetworksLoaders/dxxwjz/dxxwjz1/Dxxwjz1Reader.h"
#include "src/NetworksLoaders/dxxwjz/dxxwjz1/Dxxwjz1Writer.h"
#include "src/NetworksLoaders/original-weights/OrigWeightsReader.h"
#include "src/NetworksLoaders/original-weights/OrigWeightsWriter.h"
#include "src/NetworksLoaders/weights-model/WeightsDocument.h"

namespace dxxwjz1_test
{

using darknet_adaptor::loaders::OrigWeightsReader;
using darknet_adaptor::loaders::OrigWeightsWriter;
using darknet_adaptor::loaders::WeightsDocument;
using darknet_adaptor::loaders::dxxwjz1::v1::Dxxwjz1Reader;
using darknet_adaptor::loaders::dxxwjz1::v1::Dxxwjz1Writer;

namespace
{

/// @brief The biggest networks checked by the JSON tree of their document,
/// the bigger ones are checked by the conversion to the original weights
/// file, which is checked byte by byte.
constexpr const uint64_t DOCUMENT_TREE_FLOATS = 1024U * 1024U;

/// @brief The networks of the tens of millions of the floats (the 4K
/// resolution ones), the JSON text of which takes the minutes to write and to
/// read in the Debug build: they are checked by the fewer files.
constexpr const uint64_t HUGE_FLOATS = 4U * 1024U * 1024U;

/// @brief The size of the text chunks of the zlib gzread() and gzwrite().
constexpr const unsigned int CHUNK = 1U << 16U;

std::string hex(const uint32_t value)
{
  std::ostringstream text;
  text << "0x" << std::hex << std::setw(8) << std::setfill('0') << value;
  return text.str();
}

LayoutOptions reading(const bool transposed, const int cutoff)
{
  LayoutOptions options;
  options.loading = true;
  options.transposed = transposed;
  options.cutoff = cutoff;
  return options;
}

LayoutOptions saving(const int cutoff, const bool ema)
{
  LayoutOptions options;
  options.cutoff = cutoff;
  options.ema = ema;
  return options;
}

Pattern before_pattern(const WeightsVariant& variant)
{
  return Pattern{~variant.seed};
}

/// @brief The header of the variant the dxxwjz1 document keeps: the seen of
/// the original file of the version.
WeightsHeader stored_header(const WeightsHeader& header)
{
  WeightsHeader stored = header;
  stored.seen = header.stored_seen();
  return stored;
}

/// @brief The loaded arrays are the saved ones: the loader skips nothing and
/// transposes nothing, so the saved file loads and saves the same bytes.
bool reloads_same(const Layout& saved, const Layout& loaded)
{
  if (saved.size() != loaded.size()) {
    return false;
  }

  for (size_t index = 0U; index < saved.size(); ++index) {
    if (saved[index].layer != loaded[index].layer ||
        saved[index].sub != loaded[index].sub ||
        saved[index].array != loaded[index].array ||
        saved[index].count != loaded[index].count || loaded[index].rows != 0U) {
      return false;
    }
  }

  return true;
}

/// @brief The loader reads every array the file keeps: no dontload layers
/// and no dontloadscales batch normalization arrays, which the original
/// loader reads the next arrays in place of.
bool loads_every_array(const Layout& saved, const Layout& loaded)
{
  if (saved.size() != loaded.size()) {
    return false;
  }

  for (size_t index = 0U; index < saved.size(); ++index) {
    if (saved[index].layer != loaded[index].layer ||
        saved[index].sub != loaded[index].sub ||
        saved[index].array != loaded[index].array) {
      return false;
    }
  }

  return true;
}

/// @brief The float the JSON value keeps, false for none.
bool float_of(const fjson& value, float& result)
{
  if (value.is_number_float()) {
    result = value.get<float>();
    return true;
  }

  if (value.is_number_integer() || value.is_number_unsigned()) {
    result = value.is_number_unsigned()
                 ? static_cast<float>(value.get<uint64_t>())
                 : static_cast<float>(value.get<int64_t>());
    return true;
  }

  if (!value.is_string()) {
    return false;
  }

  const std::string text = value.get<std::string>();

  if (text.size() != 10U || text.compare(0U, 2U, "0x") != 0) {
    return false;
  }

  const uint32_t bits =
      static_cast<uint32_t>(std::stoul(text.substr(2U), nullptr, 16));

  std::memcpy(&result, &bits, sizeof(result));

  return true;
}

::testing::AssertionResult failure_of(const Block& block,
                                      const std::string& text)
{
  return ::testing::AssertionFailure() << describe(block) << ": " << text;
}

/// @brief The values of the array of the document match the block.
::testing::AssertionResult values_match(const fjson& values, const Block& block,
                                        const Pattern& pattern)
{
  if (!values.is_array()) {
    return failure_of(block, "the values aren't the array");
  }

  if (values.size() != block.count) {
    return failure_of(block, std::to_string(values.size()) +
                                 " values instead of " +
                                 std::to_string(block.count));
  }

  const PatternValues expected = pattern.of(block);

  for (uint64_t index = 0U; index < block.count; ++index) {
    float actual = 0.f;

    if (!float_of(values[index], actual)) {
      return failure_of(block, "the value " + std::to_string(index) +
                                   " isn't a float: " + values[index].dump());
    }

    if (bits(actual) != bits(expected(index))) {
      return failure_of(block, "the value " + std::to_string(index) + " " +
                                   hex(bits(actual)) + " instead of " +
                                   hex(bits(expected(index))));
    }
  }

  return ::testing::AssertionSuccess();
}

/**
 * @brief Writes the dxxwjz1 file of the first floats of the layout: the
 * independent JSON tree of the small networks, the conversion of the
 * independently written original weights file of the big ones (the JSON tree
 * takes the tens of bytes of every float).
 */
::testing::AssertionResult write_dxxwjz1(
    const std::string& path, const NetSpec& spec, const WeightsHeader& header,
    const Layout& layout, const Pattern& pattern, const uint64_t floats,
    const network& net)
{
  if (count_floats(layout) <= DOCUMENT_TREE_FLOATS) {
    if (!write_document(path,
                        document_of(spec, header, layout, pattern, floats))) {
      return ::testing::AssertionFailure() << "the write of " << path;
    }

    return ::testing::AssertionSuccess();
  }

  const std::string weights = path + ".weights";

  if (!write_weights_file(weights, header, layout, pattern) ||
      (floats < count_floats(layout) &&
       !truncate_file(weights, header.size() + floats * sizeof(float)))) {
    return ::testing::AssertionFailure() << "the write of " << weights;
  }

  WeightsDocument document;

  if (!OrigWeightsReader{}.read(weights, &net, document) ||
      !Dxxwjz1Writer{}.write(document, path)) {
    return ::testing::AssertionFailure() << "the conversion of " << weights;
  }

  std::remove(weights.c_str());

  return ::testing::AssertionSuccess();
}

void same_arrays(DarknetNetwork& first, DarknetNetwork& second,
                 const Layout& memory)
{
  for (const Block& block : memory) {
    const float* a = first.array(block);
    const float* b = second.array(block);

    ASSERT_EQ(a == nullptr, b == nullptr) << describe(block);

    if (a == nullptr) {
      continue;
    }

    for (uint64_t index = 0U; index < block.count; ++index) {
      ASSERT_EQ(bits(a[index]), bits(b[index]))
          << describe(block) << " at the index " << index;
    }
  }
}

}  // namespace

std::string type_name(const LayerKind kind)
{
  switch (kind) {
    case LayerKind::Convolutional:
      return "convolutional";
    case LayerKind::Connected:
      return "connected";
    case LayerKind::Batchnorm:
      return "batchnorm";
    case LayerKind::Shortcut:
      return "shortcut";
    case LayerKind::Implicit:
      return "implicit";
    case LayerKind::Local:
      return "local";
    case LayerKind::Rnn:
      return "rnn";
    case LayerKind::Gru:
      return "gru";
    case LayerKind::Lstm:
      return "lstm";
    case LayerKind::Crnn:
      return "crnn";
    case LayerKind::ConvLstm:
      return "conv_lstm";
    case LayerKind::Maxpool:
      return "maxpool";
    case LayerKind::LocalAvgpool:
      return "local_avgpool";
    case LayerKind::Avgpool:
      return "avgpool";
    case LayerKind::Route:
      return "route";
    case LayerKind::Upsample:
      return "upsample";
    case LayerKind::Reorg:
      // the [reorg3d] section of the generated networks
      return "reorg3d";
    case LayerKind::Dropout:
      return "dropout";
    case LayerKind::Activation:
      return "activation";
    case LayerKind::Normalization:
      return "normalization";
    case LayerKind::Empty:
      return "empty";
    case LayerKind::Softmax:
      return "softmax";
    case LayerKind::Yolo:
      return "yolo";
  }

  return "unknown";
}

std::string sub_name(const SubLayer sub)
{
  switch (sub) {
    case SubLayer::None:
      return "";
    case SubLayer::Input:
      return "input_layer";
    case SubLayer::Self:
      return "self_layer";
    case SubLayer::Output:
      return "output_layer";
    case SubLayer::InputZ:
      return "input_z_layer";
    case SubLayer::InputR:
      return "input_r_layer";
    case SubLayer::InputH:
      return "input_h_layer";
    case SubLayer::StateZ:
      return "state_z_layer";
    case SubLayer::StateR:
      return "state_r_layer";
    case SubLayer::StateH:
      return "state_h_layer";
    case SubLayer::Uf:
      return "uf";
    case SubLayer::Ui:
      return "ui";
    case SubLayer::Ug:
      return "ug";
    case SubLayer::Uo:
      return "uo";
    case SubLayer::Wf:
      return "wf";
    case SubLayer::Wi:
      return "wi";
    case SubLayer::Wg:
      return "wg";
    case SubLayer::Wo:
      return "wo";
    case SubLayer::Vf:
      return "vf";
    case SubLayer::Vi:
      return "vi";
    case SubLayer::Vo:
      return "vo";
  }

  return "unknown";
}

std::string array_name(const ArrayKind array)
{
  switch (array) {
    case ArrayKind::Biases:
    case ArrayKind::BiasesEma:
      return "biases";
    case ArrayKind::Scales:
    case ArrayKind::ScalesEma:
      return "scales";
    case ArrayKind::RollingMean:
      return "rolling_mean";
    case ArrayKind::RollingVariance:
      return "rolling_variance";
    case ArrayKind::Weights:
    case ArrayKind::WeightsEma:
      return "weights";
  }

  return "unknown";
}

fjson float_json(const float value)
{
  if (!std::isfinite(value)) {
    return fjson(hex(bits(value)));
  }

  return fjson(value);
}

DocumentCounts counts_of(const Layout& layout, const uint64_t floats)
{
  DocumentCounts counts;
  uint64_t remaining = floats;

  for (const Block& block : layout) {
    if (remaining == 0U) {
      break;
    }

    const uint64_t count = std::min(remaining, block.count);

    counts[ArrayKey{block.layer, block.sub, block.array}] = count;
    remaining -= count;
  }

  return counts;
}

fjson document_of(const NetSpec& spec, const WeightsHeader& header,
                  const Layout& layout, const Pattern& pattern,
                  const uint64_t floats, const int cutoff)
{
  fjson document = fjson::object();

  document["format"] = "dxxwjz1";
  document["version"] = 1;
  document["header"] = fjson{{"major", header.major},
                             {"minor", header.minor},
                             {"revision", header.revision},
                             {"seen", header.seen}};

  fjson layers = fjson::array();

  const int count =
      std::min(static_cast<int>(spec.layers.size()), std::max(cutoff, 0));

  for (int index = 0; index < count; ++index) {
    layers.push_back(fjson{
        {"index", index},
        {"type", type_name(spec.layers[static_cast<size_t>(index)].kind)}});
  }

  uint64_t remaining = floats;

  for (const Block& block : layout) {
    if (remaining == 0U || block.layer >= count) {
      break;
    }

    const uint64_t taken = std::min(remaining, block.count);
    const PatternValues values = pattern.of(block);

    fjson array = fjson::array();

    for (uint64_t index = 0U; index < taken; ++index) {
      array.push_back(float_json(values(index)));
    }

    fjson& layer = layers[static_cast<size_t>(block.layer)];

    if (block.sub == SubLayer::None) {
      layer["arrays"][array_name(block.array)] = std::move(array);
    } else {
      layer["sublayers"][sub_name(block.sub)]["arrays"]
           [array_name(block.array)] = std::move(array);
    }

    remaining -= taken;
  }

  document["layers"] = std::move(layers);

  return document;
}

bool write_text(const std::string& path, const std::string& text,
                const bool compressed)
{
  if (!compressed) {
    std::ofstream file{path, std::ios::binary | std::ios::trunc};
    file.write(text.data(), static_cast<std::streamsize>(text.size()));
    return static_cast<bool>(file);
  }

  // the fastest compression level, the readers take any
  gzFile file = gzopen(path.c_str(), "wb1");

  if (file == nullptr) {
    return false;
  }

  bool written = true;

  for (size_t offset = 0U; offset < text.size() && written; offset += CHUNK) {
    const auto count = static_cast<unsigned int>(
        std::min<size_t>(CHUNK, text.size() - offset));

    written =
        gzwrite(file, text.data() + offset, count) == static_cast<int>(count);
  }

  return gzclose(file) == Z_OK && written;
}

bool write_document(const std::string& path, const fjson& document,
                    const bool compressed)
{
  return write_text(path, document.dump(), compressed);
}

bool read_text(const std::string& path, std::string& text)
{
  gzFile file = gzopen(path.c_str(), "rb");

  if (file == nullptr) {
    return false;
  }

  std::vector<char> chunk(CHUNK);
  int count = 0;

  text.clear();

  while ((count = gzread(file, chunk.data(), CHUNK)) > 0) {
    text.append(chunk.data(), static_cast<size_t>(count));
  }

  int error = Z_OK;
  gzerror(file, &error);

  return gzclose(file) == Z_OK && count == 0 && error == Z_OK;
}

bool read_document(const std::string& path, fjson& document)
{
  std::string text;

  if (!read_text(path, text)) {
    return false;
  }

  document = fjson::parse(text, nullptr, false);

  return !document.is_discarded();
}

::testing::AssertionResult document_matches(
    const fjson& document, const NetSpec& spec, const WeightsHeader& header,
    const Layout& layout, const Pattern& pattern, const int cutoff)
{
  if (!document.is_object()) {
    return ::testing::AssertionFailure() << "the document isn't the object";
  }

  if (document.value("format", std::string{}) != "dxxwjz1" ||
      !document.contains("version") || document["version"] != 1) {
    return ::testing::AssertionFailure()
           << "the format or the version: " << document.value("format", "")
           << " " << document.value("version", fjson{}).dump();
  }

  const fjson expected_header{{"major", header.major},
                              {"minor", header.minor},
                              {"revision", header.revision},
                              {"seen", header.seen}};

  if (!document.contains("header") || document["header"] != expected_header) {
    return ::testing::AssertionFailure()
           << "the header " << document.value("header", fjson{}).dump()
           << " instead of " << expected_header.dump();
  }

  if (!document.contains("layers")) {
    return ::testing::AssertionFailure() << "the document has no layers";
  }

  const fjson& layers = document.at("layers");
  const auto count = static_cast<size_t>(
      std::min(static_cast<int>(spec.layers.size()), std::max(cutoff, 0)));

  if (!layers.is_array() || layers.size() != count) {
    return ::testing::AssertionFailure()
           << "the " << (layers.is_array() ? layers.size() : 0U)
           << " layers instead of " << count;
  }

  // the names the layers keep: the arrays of the layer and of every
  // sub-layer
  std::map<int, std::set<std::string>> arrays;
  std::map<int, std::map<std::string, std::set<std::string>>> subs;

  for (const Block& block : layout) {
    if (block.sub == SubLayer::None) {
      arrays[block.layer].insert(array_name(block.array));
    } else {
      subs[block.layer][sub_name(block.sub)].insert(array_name(block.array));
    }
  }

  for (size_t index = 0U; index < count; ++index) {
    const fjson& l = layers[index];
    const int i = static_cast<int>(index);

    if (!l.is_object() || l.value("index", -1) != i ||
        l.value("type", std::string{}) != type_name(spec.layers[index].kind)) {
      return ::testing::AssertionFailure()
             << "the layer " << index << " is " << l.dump().substr(0U, 200U);
    }

    std::set<std::string> names;

    if (l.contains("arrays")) {
      for (const auto& [name, values] : l.at("arrays").items()) {
        names.insert(name);
      }
    }

    if (names != arrays[i]) {
      return ::testing::AssertionFailure()
             << "the arrays of the layer " << index << " are "
             << l.value("arrays", fjson::object()).dump().substr(0U, 200U);
    }

    std::map<std::string, std::set<std::string>> sub_names;

    if (l.contains("sublayers")) {
      for (const auto& [name, sub] : l.at("sublayers").items()) {
        if (!sub.contains("arrays")) {
          continue;
        }

        for (const auto& [array, values] : sub.at("arrays").items()) {
          sub_names[name].insert(array);
        }
      }
    }

    if (sub_names != subs[i]) {
      return ::testing::AssertionFailure()
             << "the sub-layers arrays of the layer " << index;
    }
  }

  for (const Block& block : layout) {
    const fjson& l = layers[static_cast<size_t>(block.layer)];
    const fjson& values = block.sub == SubLayer::None
                              ? l["arrays"][array_name(block.array)]
                              : l["sublayers"][sub_name(block.sub)]["arrays"]
                                 [array_name(block.array)];

    const ::testing::AssertionResult matches =
        values_match(values, block, pattern);

    if (!matches) {
      return matches;
    }
  }

  return ::testing::AssertionSuccess();
}

::testing::AssertionResult holds_document(
    DarknetNetwork& darknet, const Layout& memory, const Pattern& before,
    const Layout& loaded, const Pattern& pattern, const DocumentCounts& counts)
{
  std::map<ArrayKey, const Block*> reads;

  for (const Block& block : loaded) {
    reads.emplace(ArrayKey{block.layer, block.sub, block.array}, &block);
  }

  for (const Block& block : memory) {
    const float* values = darknet.array(block);

    if (values == nullptr) {
      if (block.count == 0U) {
        continue;
      }

      return ::testing::AssertionFailure()
             << "the darknet network has no " << describe(block);
    }

    const ArrayKey key{block.layer, block.sub, block.array};
    const PatternValues old_values = before.of(block);
    const auto read = reads.find(key);
    const auto count = counts.find(key);

    std::vector<float> expected(block.count);

    for (uint64_t index = 0U; index < block.count; ++index) {
      expected[index] = old_values(index);
    }

    if (read != reads.end() && count != counts.end()) {
      const PatternValues file_values = pattern.of(*read->second);
      const uint64_t kept = std::min(count->second, block.count);

      for (uint64_t index = 0U; index < kept; ++index) {
        expected[index] = file_values(index);
      }

      if (read->second->rows != 0U) {
        const std::vector<float> source = expected;
        const uint64_t rows = read->second->rows;
        const uint64_t cols = read->second->cols;

        for (uint64_t index = 0U; index < block.count; ++index) {
          expected[index] = source[(index % rows) * cols + index / rows];
        }
      }
    }

    for (uint64_t index = 0U; index < block.count; ++index) {
      if (bits(values[index]) != bits(expected[index])) {
        return ::testing::AssertionFailure()
               << describe(block) << " holds " << values[index] << " ("
               << hex(bits(values[index])) << ") at the index " << index
               << " instead of " << expected[index] << " ("
               << hex(bits(expected[index])) << ")";
      }
    }
  }

  return ::testing::AssertionSuccess();
}

void save(network& net, const std::string& path)
{
  EXPECT_TRUE(Dxxwjz1Writer{}.save(net, path)) << "the save of " << path;
}

void save_upto(network& net, const std::string& path, int cutoff, bool ema)
{
  EXPECT_TRUE(Dxxwjz1Writer{}.save(net, path, cutoff, ema))
      << "the save of " << path;
}

void load(network& net, const std::string& path)
{
  EXPECT_TRUE(Dxxwjz1Reader{}.load(net, path)) << "the load of " << path;
}

void load_upto(network& net, const std::string& path, int cutoff)
{
  EXPECT_TRUE(Dxxwjz1Reader{}.load(net, path, cutoff))
      << "the load of " << path;
}

void check_save(const WeightsVariant& variant,
                const WeightsWorkspace& workspace)
{
  const std::string cfg = workspace.write_cfg(variant.spec);
  const std::string path = workspace.file(variant.name + ".dxxwjz1");
  const Layout layout = variant.spec.layout();
  const Pattern pattern{variant.seed};

  DarknetNetwork darknet{cfg, variant.mode};
  network& net = darknet.net();

  ASSERT_TRUE(matches_spec(net, variant.spec, variant.mode));
  ASSERT_TRUE(darknet.fill(layout, pattern));

  *net.cur_iteration = variant.iteration;

  save(net, path);

  const uint64_t seen =
      saved_seen(variant.spec, variant.mode, variant.iteration);

  EXPECT_EQ(*net.seen, seen);

  if (count_floats(layout) <= DOCUMENT_TREE_FLOATS) {
    fjson document;

    ASSERT_TRUE(read_document(path, document));
    EXPECT_TRUE(document_matches(document, variant.spec, saved_header(seen),
                                 layout, pattern));
  } else {
    // the original weights file of the document, checked byte by byte
    const std::string weights = workspace.file(variant.name + ".weights");
    WeightsDocument document;

    ASSERT_TRUE(Dxxwjz1Reader{}.read(path, &net, document));
    ASSERT_TRUE(OrigWeightsWriter{}.write(document, weights));
    EXPECT_TRUE(file_matches(weights, saved_header(seen), layout, pattern));
  }

  // the save leaves the arrays as they are
  EXPECT_TRUE(darknet.holds(layout, pattern));
}

void check_load(const WeightsVariant& variant,
                const WeightsWorkspace& workspace)
{
  const WeightsHeader header = stored_header(variant.header);
  const std::string cfg = workspace.write_cfg(variant.spec);
  const std::string path = workspace.file(variant.name + ".dxxwjz1");
  const Layout memory = variant.spec.layout();
  const Layout loaded = variant.spec.layout(
      reading(header.transposed(), std::numeric_limits<int>::max()));
  const Pattern pattern{variant.seed};
  const Pattern before = before_pattern(variant);

  DarknetNetwork darknet{cfg, variant.mode};
  network& net = darknet.net();

  ASSERT_TRUE(matches_spec(net, variant.spec, variant.mode));

  // the document of every array the network saves: the loader skips the
  // dontload and the dontloadscales ones by their names
  ASSERT_TRUE(write_dxxwjz1(path, variant.spec, header, memory, pattern,
                            ALL_FLOATS, net));
  ASSERT_TRUE(darknet.fill(memory, before));

  load(net, path);

  EXPECT_EQ(*net.seen, header.seen);
  EXPECT_EQ(*net.cur_iteration,
            loaded_iteration(variant.spec, variant.mode, header.seen));
  EXPECT_TRUE(holds_document(darknet, memory, before, loaded, pattern,
                             counts_of(memory)));
}

void check_round_trip(const WeightsVariant& variant,
                      const WeightsWorkspace& workspace)
{
  const std::string cfg = workspace.write_cfg(variant.spec);
  const std::string first = workspace.file(variant.name + ".dxxwjz1");
  const std::string second = workspace.file(variant.name + ".resaved.dxxwjz1");
  const Layout saved = variant.spec.layout();
  const Layout loaded =
      variant.spec.layout(reading(false, std::numeric_limits<int>::max()));
  const Pattern pattern{variant.seed};
  const Pattern before = before_pattern(variant);

  {
    DarknetNetwork darknet{cfg, variant.mode};

    ASSERT_TRUE(darknet.fill(saved, pattern));

    *darknet.net().cur_iteration = variant.iteration;

    save(darknet.net(), first);
  }

  DarknetNetwork darknet{cfg, variant.mode};
  network& net = darknet.net();

  ASSERT_TRUE(darknet.fill(saved, before));

  load(net, first);

  EXPECT_EQ(*net.seen,
            saved_seen(variant.spec, variant.mode, variant.iteration));
  EXPECT_EQ(*net.cur_iteration, variant.iteration);
  EXPECT_TRUE(holds_document(darknet, saved, before, loaded, pattern,
                             counts_of(saved)));

  if (reloads_same(saved, loaded)) {
    save(net, second);

    EXPECT_TRUE(files_equal(first, second));
  }
}

void check_truncated(const WeightsVariant& variant,
                     const WeightsWorkspace& workspace)
{
  const WeightsHeader header = stored_header(variant.header);
  const std::string cfg = workspace.write_cfg(variant.spec);
  const std::string path = workspace.file(variant.name + ".dxxwjz1");
  const Layout memory = variant.spec.layout();
  const Layout loaded = variant.spec.layout(
      reading(header.transposed(), std::numeric_limits<int>::max()));
  const Pattern pattern{variant.seed};
  const Pattern before = before_pattern(variant);
  const uint64_t floats = count_floats(memory);

  if (floats == 0U) {
    return;
  }

  // the empty document, the parts of the first array, the arrays edges and
  // the middle and the tail of the arrays, a part of them for the big
  // networks
  std::set<uint64_t> cuts{0U, 1U, floats / 2U, floats - 1U};

  if (floats > HUGE_FLOATS) {
    cuts = {floats / 3U};
  } else if (floats > DOCUMENT_TREE_FLOATS) {
    cuts = {floats / 3U, floats - 1U};
  }

  uint64_t edge = 0U;

  for (size_t index = 0U;
       index <
       std::min<size_t>(memory.size(), floats > DOCUMENT_TREE_FLOATS ? 0U : 3U);
       ++index) {
    edge += memory[index].count;
    cuts.insert(edge);
    cuts.insert(edge - 1U);
  }

  for (const uint64_t cut : cuts) {
    if (cut >= floats) {
      continue;
    }

    SCOPED_TRACE("the document of the first " + std::to_string(cut) + " of " +
                 std::to_string(floats) + " floats");

    DarknetNetwork darknet{cfg, variant.mode};
    network& net = darknet.net();

    ASSERT_TRUE(
        write_dxxwjz1(path, variant.spec, header, memory, pattern, cut, net));
    ASSERT_TRUE(darknet.fill(memory, before));

    load(net, path);

    EXPECT_EQ(*net.seen, header.seen);
    EXPECT_TRUE(holds_document(darknet, memory, before, loaded, pattern,
                               counts_of(memory, cut)));
  }
}

void check_cutoff(const WeightsVariant& variant,
                  const WeightsWorkspace& workspace)
{
  const int n = static_cast<int>(variant.spec.layers.size());
  const std::string cfg = workspace.write_cfg(variant.spec);
  const Layout memory = variant.spec.layout();
  const Pattern pattern{variant.seed};
  const Pattern before = before_pattern(variant);
  const uint64_t seen =
      saved_seen(variant.spec, variant.mode, variant.iteration);
  const std::set<int> cutoffs{-1, 0, 1, n / 2, n - 1, n, n + 3};
  const std::string full = workspace.file(variant.name + ".dxxwjz1");

  {
    DarknetNetwork darknet{cfg, variant.mode};

    ASSERT_TRUE(darknet.fill(memory, pattern));

    *darknet.net().cur_iteration = variant.iteration;

    save(darknet.net(), full);

    for (const int cutoff : cutoffs) {
      SCOPED_TRACE("the save cutoff " + std::to_string(cutoff));

      const std::string cut = workspace.file(
          variant.name + ".upto" + std::to_string(cutoff) + ".dxxwjz1");

      save_upto(darknet.net(), cut, cutoff, false);

      fjson document;

      ASSERT_TRUE(read_document(cut, document));
      EXPECT_TRUE(document_matches(document, variant.spec, saved_header(seen),
                                   variant.spec.layout(saving(cutoff, false)),
                                   pattern, cutoff));
    }
  }

  DarknetNetwork darknet{cfg, variant.mode};
  network& net = darknet.net();

  for (const int cutoff : cutoffs) {
    SCOPED_TRACE("the load cutoff " + std::to_string(cutoff));

    ASSERT_TRUE(darknet.fill(memory, before));

    *net.seen = 0U;

    load_upto(net, full, cutoff);

    // the header is read regardless of the cutoff
    EXPECT_EQ(*net.seen, seen);
    EXPECT_TRUE(holds_document(darknet, memory, before,
                               variant.spec.layout(reading(false, cutoff)),
                               pattern, counts_of(memory)));

    // the whole network load of the file saved up to the cutoff
    ASSERT_TRUE(darknet.fill(memory, before));

    load(net, workspace.file(variant.name + ".upto" + std::to_string(cutoff) +
                             ".dxxwjz1"));

    EXPECT_TRUE(holds_document(
        darknet, memory, before,
        variant.spec.layout(reading(false, std::numeric_limits<int>::max())),
        pattern, counts_of(variant.spec.layout(saving(cutoff, false)))));
  }
}

void check_ema(const WeightsVariant& variant, const WeightsWorkspace& workspace)
{
  ASSERT_EQ(variant.mode, ParseMode::Train)
      << "the inference networks have no EMA arrays";

  const std::string cfg = workspace.write_cfg(variant.spec);
  const std::string ema_path = workspace.file(variant.name + ".ema.dxxwjz1");
  const std::string path = workspace.file(variant.name + ".dxxwjz1");
  const Layout memory = variant.spec.layout();
  const Layout ema =
      variant.spec.layout(saving(std::numeric_limits<int>::max(), true));
  const Pattern pattern{variant.seed};

  DarknetNetwork darknet{cfg, variant.mode};
  network& net = darknet.net();

  ASSERT_TRUE(darknet.fill(memory, pattern));
  ASSERT_TRUE(darknet.fill(ema, pattern));

  *net.cur_iteration = variant.iteration;

  save_upto(net, ema_path, net.n, true);
  save(net, path);

  const uint64_t seen =
      saved_seen(variant.spec, variant.mode, variant.iteration);

  fjson ema_document;
  fjson document;

  ASSERT_TRUE(read_document(ema_path, ema_document));
  ASSERT_TRUE(read_document(path, document));

  EXPECT_TRUE(document_matches(ema_document, variant.spec, saved_header(seen),
                               ema, pattern));
  EXPECT_TRUE(document_matches(document, variant.spec, saved_header(seen),
                               memory, pattern));
}

void check_conversion(const WeightsVariant& variant,
                      const WeightsWorkspace& workspace)
{
  const std::string cfg = workspace.write_cfg(variant.spec);
  const Layout memory = variant.spec.layout();
  const Pattern pattern{variant.seed};
  const Pattern before = before_pattern(variant);

  DarknetNetwork darknet{cfg, variant.mode};
  network& net = darknet.net();

  ASSERT_TRUE(matches_spec(net, variant.spec, variant.mode));

  // the file darknet saves and the file of the variant header made for the
  // load of the network (no arrays of the dontload layers and no batch
  // normalization arrays of the dontloadscales ones, the transposed
  // connected weights of the old versions)
  const WeightsHeader variant_header = stored_header(variant.header);
  const Layout variant_layout = variant.spec.layout(
      reading(variant_header.transposed(), std::numeric_limits<int>::max()));

  const std::string saved = workspace.file(variant.name + ".saved.weights");
  const std::string made = workspace.file(variant.name + ".made.weights");

  ASSERT_TRUE(darknet.fill(memory, pattern));

  *net.cur_iteration = variant.iteration;

  ASSERT_TRUE(OrigWeightsWriter{}.save(net, saved));

  if (count_floats(memory) <= HUGE_FLOATS) {
    ASSERT_TRUE(
        write_weights_file(made, variant_header, variant_layout, pattern));
  }

  const WeightsHeader saved_file_header =
      saved_header(saved_seen(variant.spec, variant.mode, variant.iteration));

  std::vector<std::tuple<std::string, WeightsHeader, Layout>> files{
      {saved, saved_file_header, memory}};

  // the huge networks convert the file darknet saves alone, the smaller ones
  // check the headers of every version
  if (count_floats(memory) <= HUGE_FLOATS) {
    files.emplace_back(made, variant_header, variant_layout);
  }

  for (const auto& [weights, header, layout] : files) {
    SCOPED_TRACE(weights);

    const std::string dxxwjz1 = weights + ".dxxwjz1";
    const std::string again = weights + ".again.dxxwjz1";
    const std::string back = weights + ".back.weights";

    WeightsDocument document;

    ASSERT_TRUE(OrigWeightsReader{}.read(weights, &net, document));
    ASSERT_TRUE(Dxxwjz1Writer{}.write(document, dxxwjz1));

    if (count_floats(layout) <= DOCUMENT_TREE_FLOATS) {
      fjson tree;

      ASSERT_TRUE(read_document(dxxwjz1, tree));
      EXPECT_TRUE(
          document_matches(tree, variant.spec, header, layout, pattern));
    }

    // the dxxwjz1 file converts back to the same bytes and to itself
    WeightsDocument converted;

    ASSERT_TRUE(Dxxwjz1Reader{}.read(dxxwjz1, &net, converted));
    ASSERT_TRUE(OrigWeightsWriter{}.write(converted, back));
    EXPECT_TRUE(files_equal(weights, back));

    ASSERT_TRUE(Dxxwjz1Writer{}.write(converted, again));
    EXPECT_TRUE(files_equal(dxxwjz1, again));

    // the dxxwjz1 file loads the arrays by their names, which the original
    // loader reads by their positions: the same network unless the original
    // loader skips some arrays of the file
    DarknetNetwork from_weights{cfg, variant.mode};
    DarknetNetwork from_dxxwjz1{cfg, variant.mode};

    ASSERT_TRUE(from_weights.fill(memory, before));
    ASSERT_TRUE(from_dxxwjz1.fill(memory, before));

    ASSERT_TRUE(OrigWeightsReader{}.load(from_weights.net(), weights));
    ASSERT_TRUE(Dxxwjz1Reader{}.load(from_dxxwjz1.net(), dxxwjz1));

    EXPECT_EQ(*from_dxxwjz1.net().seen, *from_weights.net().seen);
    EXPECT_EQ(*from_dxxwjz1.net().cur_iteration,
              *from_weights.net().cur_iteration);

    const Layout loaded = variant.spec.layout(
        reading(header.transposed(), std::numeric_limits<int>::max()));

    if (loads_every_array(layout, loaded)) {
      same_arrays(from_weights, from_dxxwjz1, memory);
    } else {
      EXPECT_TRUE(holds_document(from_dxxwjz1, memory, before, loaded, pattern,
                                 counts_of(layout)));
    }
  }
}

}  // namespace dxxwjz1_test
