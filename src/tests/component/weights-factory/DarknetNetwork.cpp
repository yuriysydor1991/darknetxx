#include "DarknetNetwork.h"

#include <algorithm>
#include <cstdlib>
#include <iomanip>
#include <limits>
#include <map>
#include <sstream>
#include <tuple>
#include <utility>
#include <vector>

#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/OrigDefaultLoader/OrigDefaultLoader.h"

namespace weights_factory
{

namespace
{

constexpr const uint64_t CHUNK_FLOATS = 1U << 16U;

std::string hex(float value)
{
  std::ostringstream text;
  text << value << " (0x" << std::hex << std::setw(8) << std::setfill('0')
       << bits(value) << ")";
  return text.str();
}

::testing::AssertionResult mismatch(const Block& block, uint64_t index,
                                    float actual, float expected)
{
  return ::testing::AssertionFailure()
         << describe(block) << " holds " << hex(actual) << " at the index "
         << index << " instead of " << hex(expected);
}

}  // namespace

DarknetNetwork::DarknetNetwork(const std::string& cfg, ParseMode mode)
{
  darknet_adaptor::loaders::OrigDefaultLoader loader;

  mnet = mode == ParseMode::Train ? loader.parse_network_cfg(cfg)
                                  : loader.parse_network_cfg_custom(cfg, 1, 1);
}

DarknetNetwork::DarknetNetwork(
    darknet_adaptor::adaptors::ANetworkAdaptorPtr loaded)
    : mnet{std::move(loaded)}
{
}

network& DarknetNetwork::net() { return *mnet->get(); }

layer* DarknetNetwork::layer_of(const Block& block)
{
  if (block.layer < 0 || block.layer >= net().n) {
    return nullptr;
  }

  layer& l = net().layers[block.layer];

  switch (block.sub) {
    case SubLayer::None:
      return &l;
    case SubLayer::Input:
      return l.input_layer;
    case SubLayer::Self:
      return l.self_layer;
    case SubLayer::Output:
      return l.output_layer;
    case SubLayer::InputZ:
      return l.input_z_layer;
    case SubLayer::InputR:
      return l.input_r_layer;
    case SubLayer::InputH:
      return l.input_h_layer;
    case SubLayer::StateZ:
      return l.state_z_layer;
    case SubLayer::StateR:
      return l.state_r_layer;
    case SubLayer::StateH:
      return l.state_h_layer;
    case SubLayer::Uf:
      return l.uf;
    case SubLayer::Ui:
      return l.ui;
    case SubLayer::Ug:
      return l.ug;
    case SubLayer::Uo:
      return l.uo;
    case SubLayer::Wf:
      return l.wf;
    case SubLayer::Wi:
      return l.wi;
    case SubLayer::Wg:
      return l.wg;
    case SubLayer::Wo:
      return l.wo;
    case SubLayer::Vf:
      return l.vf;
    case SubLayer::Vi:
      return l.vi;
    case SubLayer::Vo:
      return l.vo;
  }

  return nullptr;
}

float* DarknetNetwork::array(const Block& block)
{
  layer* l = layer_of(block);

  if (l == nullptr) {
    return nullptr;
  }

  switch (block.array) {
    case ArrayKind::Biases:
      return l->biases;
    case ArrayKind::Scales:
      return l->scales;
    case ArrayKind::RollingMean:
      return l->rolling_mean;
    case ArrayKind::RollingVariance:
      return l->rolling_variance;
    case ArrayKind::Weights:
      return l->weights;
    case ArrayKind::BiasesEma:
      return l->biases_ema;
    case ArrayKind::ScalesEma:
      return l->scales_ema;
    case ArrayKind::WeightsEma:
      return l->weights_ema;
  }

  return nullptr;
}

::testing::AssertionResult DarknetNetwork::fill(const Layout& layout,
                                                const Pattern& pattern)
{
  for (const Block& block : layout) {
    float* values = array(block);

    if (values == nullptr) {
      if (block.count == 0U) {
        continue;
      }

      return ::testing::AssertionFailure()
             << "the darknet network has no " << describe(block);
    }

    const PatternValues pattern_values = pattern.of(block);

    for (uint64_t index = 0U; index < block.count; ++index) {
      values[index] = pattern_values(index);
    }
  }

  return ::testing::AssertionSuccess();
}

::testing::AssertionResult DarknetNetwork::holds(const Layout& layout,
                                                 const Pattern& pattern)
{
  for (const Block& block : layout) {
    const float* values = array(block);

    if (values == nullptr) {
      if (block.count == 0U) {
        continue;
      }

      return ::testing::AssertionFailure()
             << "the darknet network has no " << describe(block);
    }

    const PatternValues pattern_values = pattern.of(block);

    for (uint64_t index = 0U; index < block.count; ++index) {
      const float expected = pattern_values(index);

      if (bits(values[index]) != bits(expected)) {
        return mismatch(block, index, values[index], expected);
      }
    }
  }

  return ::testing::AssertionSuccess();
}

::testing::AssertionResult DarknetNetwork::holds_loaded(const Layout& memory,
                                                        const Pattern& before,
                                                        const Layout& loaded,
                                                        const FileModel& file,
                                                        uint64_t payload_bytes)
{
  using Key = std::tuple<int, SubLayer, ArrayKind>;

  struct Read
  {
    const Block* block;
    uint64_t offset;
  };

  const uint64_t available =
      std::min<uint64_t>(payload_bytes, file.floats() * sizeof(float));
  // the floats the file holds entirely
  const uint64_t whole = available / sizeof(float);
  // the float the file holds a part of
  const uint64_t partial = available % sizeof(float) != 0U
                               ? whole
                               : std::numeric_limits<uint64_t>::max();

  // the end of the data of every loaded layer
  std::map<int, uint64_t> layer_ends;
  uint64_t offset = 0U;

  for (const Block& block : loaded) {
    offset += block.count;
    layer_ends[block.layer] = offset;
  }

  // the loader stops after the layer, which reads past the end of the file:
  // the layers after it keep their arrays untransposed
  int last_layer = std::numeric_limits<int>::max();

  for (const auto& [layer_index, end] : layer_ends) {
    if (end * sizeof(float) > available) {
      last_layer = layer_index;
      break;
    }
  }

  std::map<Key, Read> reads;
  offset = 0U;

  for (const Block& block : loaded) {
    if (block.layer <= last_layer) {
      reads.emplace(Key{block.layer, block.sub, block.array},
                    Read{&block, offset});
    }

    offset += block.count;
  }

  std::vector<float> chunk(CHUNK_FLOATS);

  for (const Block& block : memory) {
    const float* values = array(block);

    if (values == nullptr) {
      if (block.count == 0U) {
        continue;
      }

      return ::testing::AssertionFailure()
             << "the darknet network has no " << describe(block);
    }

    const PatternValues old_values = before.of(block);
    const auto found = reads.find(Key{block.layer, block.sub, block.array});

    if (found == reads.end()) {
      // the loader skips the array
      for (uint64_t index = 0U; index < block.count; ++index) {
        if (bits(values[index]) != bits(old_values(index))) {
          return mismatch(block, index, values[index], old_values(index));
        }
      }

      continue;
    }

    const Block& read = *found->second.block;
    const uint64_t base = found->second.offset;
    // the leading floats of the array the file holds
    const uint64_t covered =
        whole > base ? std::min(block.count, whole - base) : 0U;
    const uint64_t indeterminate =
        partial >= base && partial - base < block.count
            ? partial - base
            : std::numeric_limits<uint64_t>::max();

    if (read.rows != 0U) {
      // the array the loader transposes: the values before the transposition
      std::vector<float> read_values(block.count);

      file.read(base, read_values.data(), covered);

      for (uint64_t index = covered; index < block.count; ++index) {
        read_values[index] = old_values(index);
      }

      for (uint64_t index = 0U; index < block.count; ++index) {
        const uint64_t source =
            (index % read.rows) * read.cols + index / read.rows;

        if (source == indeterminate) {
          continue;
        }

        if (bits(values[index]) != bits(read_values[source])) {
          return mismatch(block, index, values[index], read_values[source]);
        }
      }

      continue;
    }

    for (uint64_t start = 0U; start < block.count; start += CHUNK_FLOATS) {
      const uint64_t count = std::min(CHUNK_FLOATS, block.count - start);
      const uint64_t from_file =
          covered > start ? std::min(count, covered - start) : 0U;

      file.read(base + start, chunk.data(), from_file);

      for (uint64_t element = from_file; element < count; ++element) {
        chunk[element] = old_values(start + element);
      }

      for (uint64_t element = 0U; element < count; ++element) {
        const uint64_t index = start + element;

        if (index != indeterminate &&
            bits(values[index]) != bits(chunk[element])) {
          return mismatch(block, index, values[index], chunk[element]);
        }
      }
    }
  }

  return ::testing::AssertionSuccess();
}

}  // namespace weights_factory
