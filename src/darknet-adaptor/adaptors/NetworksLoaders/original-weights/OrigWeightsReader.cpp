#include "src/darknet-adaptor/adaptors/NetworksLoaders/original-weights/OrigWeightsReader.h"

#include <algorithm>
#include <cstdint>
#include <fstream>
#include <istream>
#include <memory>
#include <string>
#include <vector>

#include "src/darknet-adaptor/adaptors/NetworksLoaders/original-weights/OrigWeightsValidator.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/weights-model/WeightsDocument.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/weights-model/WeightsLayout.h"
#include "src/darknet-adaptor/adaptors/orig-includes.h"

namespace darknet_adaptor::loaders
{

namespace
{

template <typename T>
void read_value(std::istream& file, T& value)
{
  file.read(reinterpret_cast<char*>(&value), sizeof(T));
}

size_t count_of(int value)
{
  return value > 0 ? static_cast<size_t>(value) : 0U;
}

}  // namespace

OrigWeightsReader::OrigWeightsReaderPtr OrigWeightsReader::create()
{
  return std::make_shared<OrigWeightsReader>();
}

size_t OrigWeightsReader::read_floats(std::istream& file, float* buffer,
                                      size_t count)
{
  if (count == 0U) {
    return 0U;
  }

  if (buffer == nullptr) {
    // the absent array consumes it's part of the file all the same
    std::vector<float> skipped(count);
    return read_floats(file, skipped.data(), count);
  }

  file.read(reinterpret_cast<char*>(buffer),
            static_cast<std::streamsize>(count * sizeof(float)));

  return static_cast<size_t>(file.gcount()) / sizeof(float);
}

void OrigWeightsReader::transpose_matrix(float* a, int rows, int cols)
{
  WeightsLayout::transpose_matrix(a, rows, cols);
}

bool OrigWeightsReader::read_header(std::istream& file, WeightsHeader& header)
{
  read_value(file, header.major);
  read_value(file, header.minor);
  read_value(file, header.revision);

  if (header.seen64()) {
    uint64_t iseen = 0;
    read_value(file, iseen);
    header.seen = iseen;
  } else {
    uint32_t iseen = 0;
    read_value(file, iseen);
    header.seen = iseen;
  }

  return static_cast<bool>(file);
}

void OrigWeightsReader::load_connected_weights(const layer& l,
                                               std::istream& file,
                                               bool transpose)
{
  read_floats(file, l.biases, count_of(l.outputs));
  read_floats(file, l.weights, count_of(l.outputs) * count_of(l.inputs));
  if (transpose) {
    transpose_matrix(l.weights, l.inputs, l.outputs);
  }
  if (l.batch_normalize && (!l.dontloadscales)) {
    read_floats(file, l.scales, count_of(l.outputs));
    read_floats(file, l.rolling_mean, count_of(l.outputs));
    read_floats(file, l.rolling_variance, count_of(l.outputs));
  }
}

void OrigWeightsReader::load_batchnorm_weights(const layer& l,
                                               std::istream& file)
{
  read_floats(file, l.biases, count_of(l.c));
  read_floats(file, l.scales, count_of(l.c));
  read_floats(file, l.rolling_mean, count_of(l.c));
  read_floats(file, l.rolling_variance, count_of(l.c));
}

void OrigWeightsReader::load_convolutional_weights_binary(const layer& l,
                                                          std::istream& file)
{
  read_floats(file, l.biases, count_of(l.n));
  if (l.batch_normalize && (!l.dontloadscales)) {
    read_floats(file, l.scales, count_of(l.n));
    read_floats(file, l.rolling_mean, count_of(l.n));
    read_floats(file, l.rolling_variance, count_of(l.n));
  }

  const int size = (l.c / l.groups) * l.size * l.size;

  for (int i = 0; i < l.n; ++i) {
    float mean = 0;
    read_value(file, mean);

    for (int j = 0; j < size / 8; ++j) {
      const int index = i * size + j * 8;
      unsigned char c = 0;
      read_value(file, c);

      for (int k = 0; k < 8; ++k) {
        if (j * 8 + k >= size) break;
        l.weights[index + k] = (c & 1 << k) ? mean : -mean;
      }
    }
  }
}

void OrigWeightsReader::load_convolutional_weights(const layer& l,
                                                   std::istream& file)
{
  // the binary layers keep their float weights, as the original loaded them
  const size_t num = count_of(l.nweights);

  size_t read_count = read_floats(file, l.biases, count_of(l.n));
  if (read_count > 0 && read_count < count_of(l.n)) {
    CLOGW("Warning: Unexpected end of wights-file! l.biases - l.index = %d",
          l.index);
  }

  if (l.batch_normalize && (!l.dontloadscales)) {
    read_count = read_floats(file, l.scales, count_of(l.n));
    if (read_count > 0 && read_count < count_of(l.n)) {
      CLOGW("Warning: Unexpected end of wights-file! l.scales - l.index = %d",
            l.index);
    }

    read_count = read_floats(file, l.rolling_mean, count_of(l.n));
    if (read_count > 0 && read_count < count_of(l.n)) {
      CLOGW(
          "Warning: Unexpected end of wights-file! l.rolling_mean - l.index "
          "= %d",
          l.index);
    }

    read_count = read_floats(file, l.rolling_variance, count_of(l.n));
    if (read_count > 0 && read_count < count_of(l.n)) {
      CLOGW(
          "Warning: Unexpected end of wights-file! l.rolling_variance - "
          "l.index = %d",
          l.index);
    }
  }

  read_count = read_floats(file, l.weights, num);
  // the original compares the weights read with the filters count
  if (read_count > 0 && read_count < count_of(l.n)) {
    CLOGW("Warning: Unexpected end of wights-file! l.weights - l.index = %d",
          l.index);
  }

  if (l.flipped) {
    transpose_matrix(l.weights, (l.c / l.groups) * l.size * l.size, l.n);
  }
}

void OrigWeightsReader::load_shortcut_weights(const layer& l,
                                              std::istream& file)
{
  const size_t num = count_of(l.nweights);
  const size_t read_count = read_floats(file, l.weights, num);

  if (read_count > 0 && read_count < num) {
    CLOGW("Warning: Unexpected end of wights-file! l.weights - l.index = %d",
          l.index);
  }
}

void OrigWeightsReader::load_implicit_weights(const layer& l,
                                              std::istream& file)
{
  const size_t num = count_of(l.nweights);
  const size_t read_count = read_floats(file, l.weights, num);

  if (read_count > 0 && read_count < num) {
    CLOGW("Warning: Unexpected end of wights-file! l.weights - l.index = %d",
          l.index);
  }
}

bool OrigWeightsReader::load(network& net, const std::string& filename,
                             int cutoff)
{
  CLOGI("Loading weights from %s...", filename.c_str());

  std::ifstream file{filename, std::ios::binary};

  if (!file.is_open()) {
    CLOGE("Couldn't open file: %s", filename.c_str());
    return false;
  }

  WeightsHeader header;

  read_header(file, header);

  CLOGIM(header.seen64() ? " seen 64" : " seen 32");

  *net.seen = header.seen;
  *net.cur_iteration = get_current_batch(net);

  CLOGI(" trained: %.0f K-images (%.0f Kilo-batches_64)",
        static_cast<double>(static_cast<float>(*net.seen / 1000)),
        static_cast<double>(static_cast<float>(*net.seen / 64000)));

  const bool transpose = header.transposed();

  int i = 0;

  for (i = 0; i < net.n && i < cutoff; ++i) {
    const layer& l = net.layers[i];

    if (l.dontload) continue;

    if (l.type == CONVOLUTIONAL && l.share_layer == NULL) {
      load_convolutional_weights(l, file);
    }
    if (l.type == SHORTCUT && l.nweights > 0) {
      load_shortcut_weights(l, file);
    }
    if (l.type == IMPLICIT) {
      load_implicit_weights(l, file);
    }
    if (l.type == CONNECTED) {
      load_connected_weights(l, file, transpose);
    }
    if (l.type == BATCHNORM) {
      load_batchnorm_weights(l, file);
    }
    if (l.type == CRNN) {
      load_convolutional_weights(*(l.input_layer), file);
      load_convolutional_weights(*(l.self_layer), file);
      load_convolutional_weights(*(l.output_layer), file);
    }
    if (l.type == RNN) {
      load_connected_weights(*(l.input_layer), file, transpose);
      load_connected_weights(*(l.self_layer), file, transpose);
      load_connected_weights(*(l.output_layer), file, transpose);
    }
    if (l.type == GRU) {
      load_connected_weights(*(l.input_z_layer), file, transpose);
      load_connected_weights(*(l.input_r_layer), file, transpose);
      load_connected_weights(*(l.input_h_layer), file, transpose);
      load_connected_weights(*(l.state_z_layer), file, transpose);
      load_connected_weights(*(l.state_r_layer), file, transpose);
      load_connected_weights(*(l.state_h_layer), file, transpose);
    }
    if (l.type == LSTM) {
      load_connected_weights(*(l.wf), file, transpose);
      load_connected_weights(*(l.wi), file, transpose);
      load_connected_weights(*(l.wg), file, transpose);
      load_connected_weights(*(l.wo), file, transpose);
      load_connected_weights(*(l.uf), file, transpose);
      load_connected_weights(*(l.ui), file, transpose);
      load_connected_weights(*(l.ug), file, transpose);
      load_connected_weights(*(l.uo), file, transpose);
    }
    if (l.type == CONV_LSTM) {
      if (l.peephole) {
        load_convolutional_weights(*(l.vf), file);
        load_convolutional_weights(*(l.vi), file);
        load_convolutional_weights(*(l.vo), file);
      }
      load_convolutional_weights(*(l.wf), file);
      if (!l.bottleneck) {
        load_convolutional_weights(*(l.wi), file);
        load_convolutional_weights(*(l.wg), file);
        load_convolutional_weights(*(l.wo), file);
      }
      load_convolutional_weights(*(l.uf), file);
      load_convolutional_weights(*(l.ui), file);
      load_convolutional_weights(*(l.ug), file);
      load_convolutional_weights(*(l.uo), file);
    }
    if (l.type == LOCAL) {
      const size_t locations = count_of(l.out_w) * count_of(l.out_h);
      const size_t size = count_of(l.size) * count_of(l.size) * count_of(l.c) *
                          count_of(l.n) * locations;
      read_floats(file, l.biases, count_of(l.outputs));
      read_floats(file, l.weights, size);
    }

    if (file.eof()) break;
  }

  CLOGI("Done! Loaded %d layers from weights-file", i);

  return true;
}

bool OrigWeightsReader::read(const std::string& filename, const network* net,
                             WeightsDocument& document)
{
  if (net == nullptr) {
    CLOGE(
        "The original weights file %s keeps no structure, its network is "
        "required to read it",
        filename.c_str());
    return false;
  }

  OrigWeightsInspection inspection;

  if (!OrigWeightsValidator::inspect(filename, *net, inspection)) {
    return false;
  }

  std::ifstream file{filename, std::ios::binary};
  WeightsHeader header;

  if (!file.is_open() || !read_header(file, header)) {
    CLOGE("Couldn't read the header of the weights file: %s", filename.c_str());
    return false;
  }

  WeightsLayoutOptions options;
  options.loading = inspection.loading_layout;

  document = WeightsDocument{};
  document.header = header;

  for (int i = 0; i < net->n; ++i) {
    WeightsLayer l;
    l.index = i;
    l.type = net->layers[i].type;
    document.layers.push_back(l);
  }

  uint64_t remaining = inspection.payload;

  for (const WeightsLayoutEntry& entry : WeightsLayout::of(*net, options)) {
    const uint64_t whole = remaining / sizeof(float);

    if (whole == 0U) {
      break;
    }

    WeightsArray array;
    array.sub = entry.sub;
    array.kind = entry.kind;
    array.values.resize(static_cast<size_t>(
        std::min<uint64_t>(static_cast<uint64_t>(entry.count), whole)));

    const size_t count =
        read_floats(file, array.values.data(), array.values.size());

    if (count != array.values.size()) {
      CLOGE("Failure while reading the weights file: %s", filename.c_str());
      return false;
    }

    remaining -= count * sizeof(float);

    const bool truncated = count < entry.count;

    document.layers[static_cast<size_t>(entry.layer)].arrays.push_back(
        std::move(array));

    // the truncated file ends in the array
    if (truncated) {
      break;
    }
  }

  document.trailing.resize(static_cast<size_t>(remaining));

  if (remaining > 0U &&
      !file.read(reinterpret_cast<char*>(document.trailing.data()),
                 static_cast<std::streamsize>(remaining))) {
    CLOGE("Failure while reading the weights file: %s", filename.c_str());
    return false;
  }

  return true;
}

}  // namespace darknet_adaptor::loaders
