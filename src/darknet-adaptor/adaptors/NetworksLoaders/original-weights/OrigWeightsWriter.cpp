#include "src/darknet-adaptor/adaptors/NetworksLoaders/original-weights/OrigWeightsWriter.h"

#include <cstdint>
#include <fstream>
#include <memory>
#include <ostream>
#include <string>
#include <vector>

#include "src/darknet-adaptor/adaptors/NetworksLoaders/weights-model/WeightsDocument.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/weights-model/WeightsDocumentValidator.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/weights-model/WeightsReport.h"
#include "src/darknet-adaptor/adaptors/orig-includes.h"

namespace darknet_adaptor::loaders
{

namespace
{

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

OrigWeightsWriter::OrigWeightsWriterPtr OrigWeightsWriter::create()
{
  return std::make_shared<OrigWeightsWriter>();
}

void OrigWeightsWriter::write_floats(std::ostream& file, const float* buffer,
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

void OrigWeightsWriter::write_header(std::ostream& file,
                                     const WeightsHeader& header)
{
  write_value(file, header.major);
  write_value(file, header.minor);
  write_value(file, header.revision);

  if (header.seen64()) {
    write_value(file, header.seen);
  } else {
    write_value(file, static_cast<uint32_t>(header.seen));
  }
}

void OrigWeightsWriter::save_convolutional_weights_binary(const layer& l,
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

void OrigWeightsWriter::save_shortcut_weights(const layer& l,
                                              std::ostream& file)
{
  for (int i = 0; i < l.nweights; ++i) {
    CLOGI(" l.weights[%d] = %f, ", i, static_cast<double>(l.weights[i]));
  }
  CLOGI(" l.nweights = %d", l.nweights);

  write_floats(file, l.weights, count_of(l.nweights));
}

void OrigWeightsWriter::save_implicit_weights(const layer& l,
                                              std::ostream& file)
{
  write_floats(file, l.weights, count_of(l.nweights));
}

void OrigWeightsWriter::save_convolutional_weights(const layer& l,
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

void OrigWeightsWriter::save_convolutional_weights_ema(const layer& l,
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

void OrigWeightsWriter::save_batchnorm_weights(const layer& l,
                                               std::ostream& file)
{
  write_floats(file, l.biases, count_of(l.c));
  write_floats(file, l.scales, count_of(l.c));
  write_floats(file, l.rolling_mean, count_of(l.c));
  write_floats(file, l.rolling_variance, count_of(l.c));
}

void OrigWeightsWriter::save_connected_weights(const layer& l,
                                               std::ostream& file)
{
  write_floats(file, l.biases, count_of(l.outputs));
  write_floats(file, l.weights, count_of(l.outputs) * count_of(l.inputs));
  if (l.batch_normalize) {
    write_floats(file, l.scales, count_of(l.outputs));
    write_floats(file, l.rolling_mean, count_of(l.outputs));
    write_floats(file, l.rolling_variance, count_of(l.outputs));
  }
}

bool OrigWeightsWriter::save(network& net, const std::string& filename,
                             int cutoff, bool save_ema)
{
  CLOGI("Saving weights to %s", filename.c_str());

  std::ofstream file{filename, std::ios::binary | std::ios::trunc};

  if (!file.is_open()) {
    CLOGE("Couldn't open file: %s", filename.c_str());
    return false;
  }

  // remove this line, when you will save to weights-file both: seen &
  // cur_iteration
  (*net.seen) = static_cast<uint64_t>(get_current_iteration(net) * net.batch *
                                      net.subdivisions);

  write_header(file, WeightsHeader::current(*net.seen));

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

bool OrigWeightsWriter::write(const WeightsDocument& document,
                              const std::string& filename)
{
  const WeightsReport report = WeightsDocumentValidator::validate(document);

  if (!report.ok()) {
    report.log(filename);
    CLOGE("The weights document of the errors isn't written: %s",
          filename.c_str());
    return false;
  }

  std::ofstream file{filename, std::ios::binary | std::ios::trunc};

  if (!file.is_open()) {
    CLOGE("Couldn't open file: %s", filename.c_str());
    return false;
  }

  write_header(file, document.header);

  for (const WeightsLayer& l : document.layers) {
    for (const WeightsArray& a : l.arrays) {
      write_floats(file, a.values.data(), a.values.size());
    }
  }

  if (!document.trailing.empty()) {
    file.write(reinterpret_cast<const char*>(document.trailing.data()),
               static_cast<std::streamsize>(document.trailing.size()));
  }

  file.close();

  if (file.fail()) {
    CLOGE("Failure while writing the weights file: %s", filename.c_str());
    return false;
  }

  return true;
}

}  // namespace darknet_adaptor::loaders
