#include "src/darknet-adaptor/adaptors/NetworksLoaders/OrigWeightsFile/OrigWeightsFile.h"

#include <cstdint>
#include <cstring>
#include <fstream>
#include <istream>
#include <ostream>
#include <string>
#include <vector>

#include "src/darknet-adaptor/adaptors/ANetworkAdaptor/ANetworkAdaptor.h"
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

template <typename T>
void write_value(std::ostream& file, const T& value)
{
  file.write(reinterpret_cast<const char*>(&value), sizeof(T));
}

size_t count_of(int value)
{
  return value > 0 ? static_cast<size_t>(value) : 0U;
}

}  // namespace

size_t OrigWeightsFile::read_floats(std::istream& file, float* buffer,
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

void OrigWeightsFile::write_floats(std::ostream& file, const float* buffer,
                                   size_t count)
{
  if (count == 0U) {
    return;
  }

  if (buffer == nullptr) {
    const std::vector<float> zeros(count, 0.f);
    write_floats(file, zeros.data(), count);
    return;
  }

  file.write(reinterpret_cast<const char*>(buffer),
             static_cast<std::streamsize>(count * sizeof(float)));
}

void OrigWeightsFile::transpose_matrix(float* a, int rows, int cols)
{
  const size_t row_count = count_of(rows);
  const size_t col_count = count_of(cols);
  const size_t size = row_count * col_count;
  std::vector<float> transpose(size);

  for (size_t x = 0; x < row_count; ++x) {
    for (size_t y = 0; y < col_count; ++y) {
      transpose[y * row_count + x] = a[x * col_count + y];
    }
  }

  std::memcpy(a, transpose.data(), size * sizeof(float));
}

void OrigWeightsFile::save_convolutional_weights_binary(const layer& l,
                                                        std::ostream& file)
{
  const int size = (l.c / l.groups) * l.size * l.size;
  binarize_weights(l.weights, l.n, size, l.binary_weights);

  write_floats(file, l.biases, count_of(l.n));
  if (l.batch_normalize) {
    write_floats(file, l.scales, count_of(l.n));
    write_floats(file, l.rolling_mean, count_of(l.n));
    write_floats(file, l.rolling_variance, count_of(l.n));
  }

  for (int i = 0; i < l.n; ++i) {
    float mean = l.binary_weights[i * size];
    if (mean < 0) mean = -mean;
    write_value(file, mean);

    for (int j = 0; j < size / 8; ++j) {
      const int index = i * size + j * 8;
      unsigned char c = 0;

      for (int k = 0; k < 8; ++k) {
        if (j * 8 + k >= size) break;
        if (l.binary_weights[index + k] > 0) c = (c | 1 << k);
      }

      write_value(file, c);
    }
  }
}

void OrigWeightsFile::save_shortcut_weights(const layer& l, std::ostream& file)
{
  for (int i = 0; i < l.nweights; ++i) {
    CLOGI(" l.weights[%d] = %f, ", i, static_cast<double>(l.weights[i]));
  }
  CLOGI(" l.nweights = %d", l.nweights);

  write_floats(file, l.weights, count_of(l.nweights));
}

void OrigWeightsFile::save_implicit_weights(const layer& l, std::ostream& file)
{
  write_floats(file, l.weights, count_of(l.nweights));
}

void OrigWeightsFile::save_convolutional_weights(const layer& l,
                                                 std::ostream& file)
{
  // the binary layers keep their float weights, as the original saved them
  write_floats(file, l.biases, count_of(l.n));
  if (l.batch_normalize) {
    write_floats(file, l.scales, count_of(l.n));
    write_floats(file, l.rolling_mean, count_of(l.n));
    write_floats(file, l.rolling_variance, count_of(l.n));
  }
  write_floats(file, l.weights, count_of(l.nweights));
}

void OrigWeightsFile::save_convolutional_weights_ema(const layer& l,
                                                     std::ostream& file)
{
  write_floats(file, l.biases_ema, count_of(l.n));
  if (l.batch_normalize) {
    write_floats(file, l.scales_ema, count_of(l.n));
    write_floats(file, l.rolling_mean, count_of(l.n));
    write_floats(file, l.rolling_variance, count_of(l.n));
  }
  write_floats(file, l.weights_ema, count_of(l.nweights));
}

void OrigWeightsFile::save_batchnorm_weights(const layer& l, std::ostream& file)
{
  write_floats(file, l.biases, count_of(l.c));
  write_floats(file, l.scales, count_of(l.c));
  write_floats(file, l.rolling_mean, count_of(l.c));
  write_floats(file, l.rolling_variance, count_of(l.c));
}

void OrigWeightsFile::save_connected_weights(const layer& l, std::ostream& file)
{
  write_floats(file, l.biases, count_of(l.outputs));
  write_floats(file, l.weights, count_of(l.outputs) * count_of(l.inputs));
  if (l.batch_normalize) {
    write_floats(file, l.scales, count_of(l.outputs));
    write_floats(file, l.rolling_mean, count_of(l.outputs));
    write_floats(file, l.rolling_variance, count_of(l.outputs));
  }
}

bool OrigWeightsFile::save(network& net, const std::string& filename,
                           int cutoff, bool save_ema)
{
  CLOGI("Saving weights to %s", filename.c_str());

  std::ofstream file{filename, std::ios::binary | std::ios::trunc};

  if (!file.is_open()) {
    CLOGE("Couldn't open file: %s", filename.c_str());
    return false;
  }

  const int32_t major = MAJOR_VERSION;
  const int32_t minor = MINOR_VERSION;
  const int32_t revision = PATCH_VERSION;
  write_value(file, major);
  write_value(file, minor);
  write_value(file, revision);

  // remove this line, when you will save to weights-file both: seen &
  // cur_iteration
  (*net.seen) = static_cast<uint64_t>(get_current_iteration(net) * net.batch *
                                      net.subdivisions);
  write_value(file, *net.seen);

  for (int i = 0; i < net.n && i < cutoff; ++i) {
    const layer& l = net.layers[i];

    if (l.type == CONVOLUTIONAL && l.share_layer == NULL) {
      if (save_ema) {
        save_convolutional_weights_ema(l, file);
      } else {
        save_convolutional_weights(l, file);
      }
    }
    if (l.type == SHORTCUT && l.nweights > 0) {
      save_shortcut_weights(l, file);
    }
    if (l.type == IMPLICIT) {
      save_implicit_weights(l, file);
    }
    if (l.type == CONNECTED) {
      save_connected_weights(l, file);
    }
    if (l.type == BATCHNORM) {
      save_batchnorm_weights(l, file);
    }
    if (l.type == RNN) {
      save_connected_weights(*(l.input_layer), file);
      save_connected_weights(*(l.self_layer), file);
      save_connected_weights(*(l.output_layer), file);
    }
    if (l.type == GRU) {
      save_connected_weights(*(l.input_z_layer), file);
      save_connected_weights(*(l.input_r_layer), file);
      save_connected_weights(*(l.input_h_layer), file);
      save_connected_weights(*(l.state_z_layer), file);
      save_connected_weights(*(l.state_r_layer), file);
      save_connected_weights(*(l.state_h_layer), file);
    }
    if (l.type == LSTM) {
      save_connected_weights(*(l.wf), file);
      save_connected_weights(*(l.wi), file);
      save_connected_weights(*(l.wg), file);
      save_connected_weights(*(l.wo), file);
      save_connected_weights(*(l.uf), file);
      save_connected_weights(*(l.ui), file);
      save_connected_weights(*(l.ug), file);
      save_connected_weights(*(l.uo), file);
    }
    if (l.type == CONV_LSTM) {
      if (l.peephole) {
        save_convolutional_weights(*(l.vf), file);
        save_convolutional_weights(*(l.vi), file);
        save_convolutional_weights(*(l.vo), file);
      }
      save_convolutional_weights(*(l.wf), file);
      if (!l.bottleneck) {
        save_convolutional_weights(*(l.wi), file);
        save_convolutional_weights(*(l.wg), file);
        save_convolutional_weights(*(l.wo), file);
      }
      save_convolutional_weights(*(l.uf), file);
      save_convolutional_weights(*(l.ui), file);
      save_convolutional_weights(*(l.ug), file);
      save_convolutional_weights(*(l.uo), file);
    }
    if (l.type == CRNN) {
      save_convolutional_weights(*(l.input_layer), file);
      save_convolutional_weights(*(l.self_layer), file);
      save_convolutional_weights(*(l.output_layer), file);
    }
    if (l.type == LOCAL) {
      const size_t locations = count_of(l.out_w) * count_of(l.out_h);
      const size_t size = count_of(l.size) * count_of(l.size) * count_of(l.c) *
                          count_of(l.n) * locations;
      write_floats(file, l.biases, count_of(l.outputs));
      write_floats(file, l.weights, size);
    }

    file.flush();
  }

  file.close();

  if (file.fail()) {
    CLOGE("Failure while writing the weights file: %s", filename.c_str());
    return false;
  }

  return true;
}

bool OrigWeightsFile::save(network& net, const std::string& filename)
{
  return save(net, filename, net.n, false);
}

bool OrigWeightsFile::save(adaptors::ANetworkAdaptor& net,
                           const std::string& filename)
{
  return save(*net.get(), filename);
}

void OrigWeightsFile::load_connected_weights(const layer& l, std::istream& file,
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

void OrigWeightsFile::load_batchnorm_weights(const layer& l, std::istream& file)
{
  read_floats(file, l.biases, count_of(l.c));
  read_floats(file, l.scales, count_of(l.c));
  read_floats(file, l.rolling_mean, count_of(l.c));
  read_floats(file, l.rolling_variance, count_of(l.c));
}

void OrigWeightsFile::load_convolutional_weights_binary(const layer& l,
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

void OrigWeightsFile::load_convolutional_weights(const layer& l,
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

void OrigWeightsFile::load_shortcut_weights(const layer& l, std::istream& file)
{
  const size_t num = count_of(l.nweights);
  const size_t read_count = read_floats(file, l.weights, num);

  if (read_count > 0 && read_count < num) {
    CLOGW("Warning: Unexpected end of wights-file! l.weights - l.index = %d",
          l.index);
  }
}

void OrigWeightsFile::load_implicit_weights(const layer& l, std::istream& file)
{
  const size_t num = count_of(l.nweights);
  const size_t read_count = read_floats(file, l.weights, num);

  if (read_count > 0 && read_count < num) {
    CLOGW("Warning: Unexpected end of wights-file! l.weights - l.index = %d",
          l.index);
  }
}

bool OrigWeightsFile::load(network& net, const std::string& filename,
                           int cutoff)
{
  CLOGI("Loading weights from %s...", filename.c_str());

  std::ifstream file{filename, std::ios::binary};

  if (!file.is_open()) {
    CLOGE("Couldn't open file: %s", filename.c_str());
    return false;
  }

  int32_t major = 0;
  int32_t minor = 0;
  int32_t revision = 0;
  read_value(file, major);
  read_value(file, minor);
  read_value(file, revision);

  if ((major * 10 + minor) >= 2) {
    CLOGIM(" seen 64");
    uint64_t iseen = 0;
    read_value(file, iseen);
    *net.seen = iseen;
  } else {
    CLOGIM(" seen 32");
    uint32_t iseen = 0;
    read_value(file, iseen);
    *net.seen = iseen;
  }

  *net.cur_iteration = get_current_batch(net);

  CLOGI(" trained: %.0f K-images (%.0f Kilo-batches_64)",
        static_cast<double>(static_cast<float>(*net.seen / 1000)),
        static_cast<double>(static_cast<float>(*net.seen / 64000)));

  const bool transpose = (major > 1000) || (minor > 1000);

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

bool OrigWeightsFile::load(network& net, const std::string& filename)
{
  return load(net, filename, net.n);
}

bool OrigWeightsFile::load(adaptors::ANetworkAdaptor& net,
                           const std::string& filename)
{
  return load(*net.get(), filename);
}

}  // namespace darknet_adaptor::loaders
