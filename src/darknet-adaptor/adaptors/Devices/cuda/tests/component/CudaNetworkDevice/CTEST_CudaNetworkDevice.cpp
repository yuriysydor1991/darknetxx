#include <gtest/gtest.h>

#include <algorithm>
#include <cmath>
#include <initializer_list>
#include <random>
#include <string>
#include <vector>

#include "DarknetNetwork.h"
#include "WeightsFile.h"
#include "WeightsVariants.h"
#include "src/NetworksLoaders/OrigDefaultLoader/OrigDefaultLoader.h"
#include "src/darknet-adaptor/adaptors/Devices/NetworkDevices.h"
#include "src/darknet-adaptor/adaptors/orig-includes.h"

using namespace testing;
using namespace weights_factory;

using darknet_adaptor::adaptors::ANetworkAdaptorPtr;
using darknet_adaptor::adaptors::devices::INetworkDevice;
using darknet_adaptor::adaptors::devices::NetworkDevices;
using darknet_adaptor::loaders::OrigDefaultLoader;

namespace
{

/// @brief The networks of the variant on the CPU and on the device of the
/// build (the CPU too on the machine of no GPU), of the same weights.
class Networks
{
 public:
  explicit Networks(const WeightsVariant& variant)
      : mvariant{variant}, mdevice{NetworkDevices::current()}, mgpu{gpu_index}
  {
    on_cpu([&] { cpu = make(NetworkDevices::host()); });
    gpu = make(mdevice);
  }

  /// @brief Computes the CPU network with the darknet code of the CPU.
  template <typename Computation>
  void on_cpu(Computation computation)
  {
    gpu_index = -1;
    computation();
    gpu_index = mgpu;
  }

  ANetworkAdaptorPtr cpu;
  ANetworkAdaptorPtr gpu;

 private:
  ANetworkAdaptorPtr make(const INetworkDevice& device) const
  {
    const int batch = mvariant.train() ? 0 : 1;
    ANetworkAdaptorPtr net = OrigDefaultLoader{}.parse_network_text(
        mvariant.spec.cfg(), mvariant.name, batch, batch, device);

    EXPECT_TRUE(DarknetNetwork{net}.fill(mvariant.spec.layout(),
                                         Pattern{mvariant.seed}));
    device.push(*net->get());

    if (!mvariant.train()) {
      net->fuse_conv_batchnorm();
      net->calculate_binary_weights();
    }

    return net;
  }

  const WeightsVariant& mvariant;
  const INetworkDevice& mdevice;
  int mgpu;
};

std::vector<float> values(size_t count, unsigned int seed)
{
  std::mt19937 random{seed};
  std::uniform_real_distribution<float> uniform{-1.f, 1.f};
  std::vector<float> result(count);

  std::generate(result.begin(), result.end(), [&] { return uniform(random); });

  return result;
}

std::vector<float> predict(network& net, std::vector<float> input)
{
  const float* output = network_predict(net, input.data());

  return {output, output + get_network_output_size(net) * net.batch};
}

std::vector<float> input_of(const network& net)
{
  return values(static_cast<size_t>(net.w * net.h * net.c * net.batch), 7U);
}

/// @brief The values of the GPU computation are the CPU ones but the
/// rounding of the other summation order and of the half precision.
AssertionResult close(const std::vector<float>& actual,
                      const std::vector<float>& expected)
{
  if (actual.size() != expected.size()) {
    return AssertionFailure()
           << actual.size() << " values, expected " << expected.size();
  }

  for (size_t i = 0; i < actual.size(); ++i) {
    const float tolerance = 0.05f * std::max(1.f, std::fabs(expected[i]));

    if (!(std::fabs(actual[i] - expected[i]) <= tolerance)) {
      return AssertionFailure() << "value " << i << ": " << actual[i]
                                << ", expected " << expected[i];
    }
  }

  return AssertionSuccess();
}

/// @brief The weights updates of the own weights of the layers, which pull
/// brings them from the device.
std::vector<float> updates(const network& net)
{
  std::vector<float> all;

  for (int i = 0; i < net.n; ++i) {
    const layer& l = net.layers[i];
    const bool pulled = l.type == CONVOLUTIONAL || l.type == CONNECTED ||
                        l.type == SHORTCUT || l.type == IMPLICIT;

    if (pulled && l.weight_updates && !l.share_layer) {
      all.insert(all.end(), l.weight_updates, l.weight_updates + l.nweights);
    }
  }

  return all;
}

bool has(const NetSpec& spec, std::initializer_list<LayerKind> kinds)
{
  return std::any_of(
      spec.layers.begin(), spec.layers.end(), [kinds](const LayerSpec& l) {
        return std::find(kinds.begin(), kinds.end(), l.kind) != kinds.end();
      });
}

/// @brief The variants the CPU computes, the bottleneck [conv_lstm] aside.
std::vector<WeightsVariant> computed(bool train, bool resizable)
{
  std::vector<WeightsVariant> all;

  for (const WeightsVariant& variant : variants::layers()) {
    const NetSpec& spec = variant.spec;
    const bool bottleneck =
        std::any_of(spec.layers.begin(), spec.layers.end(),
                    [](const LayerSpec& l) { return l.bottleneck; });
    // the random dropout masks of the training differ by the device
    const bool random = train && has(spec, {LayerKind::Dropout});
    const bool fixed =
        resizable &&
        (!spec.has_2d_layer() ||
         has(spec,
             {LayerKind::Connected, LayerKind::Local, LayerKind::Rnn,
              LayerKind::Gru, LayerKind::Lstm, LayerKind::Implicit,
              LayerKind::Activation, LayerKind::Softmax, LayerKind::Empty}));

    if (!bottleneck && !random && !fixed && (!train || variant.train())) {
      all.push_back(variant);
    }
  }

  return all;
}

std::string variant_name(const TestParamInfo<WeightsVariant>& info)
{
  return info.param.name;
}

}  // namespace

class CTEST_CudaNetworkDevice : public TestWithParam<WeightsVariant>
{
};

TEST_P(CTEST_CudaNetworkDevice, predicts_as_the_cpu)
{
  Networks nets{GetParam()};
  const std::vector<float> input = input_of(*nets.cpu->get());
  std::vector<float> expected;

  nets.on_cpu([&] { expected = predict(*nets.cpu->get(), input); });

  EXPECT_TRUE(close(predict(*nets.gpu->get(), input), expected));
}

INSTANTIATE_TEST_SUITE_P(Generated, CTEST_CudaNetworkDevice,
                         ValuesIn(computed(false, false)), variant_name);

class CTEST_CudaNetworkDevice_resize : public CTEST_CudaNetworkDevice
{
};

TEST_P(CTEST_CudaNetworkDevice_resize, predicts_as_the_cpu_after_the_resize)
{
  Networks nets{GetParam()};
  const NetSpec& spec = GetParam().spec;
  std::vector<float> expected;

  nets.on_cpu([&] { nets.cpu->resize(spec.width * 2, spec.height * 2); });
  nets.gpu->resize(spec.width * 2, spec.height * 2);

  const std::vector<float> input = input_of(*nets.cpu->get());

  nets.on_cpu([&] { expected = predict(*nets.cpu->get(), input); });

  EXPECT_TRUE(close(predict(*nets.gpu->get(), input), expected));
}

INSTANTIATE_TEST_SUITE_P(Generated, CTEST_CudaNetworkDevice_resize,
                         ValuesIn(computed(false, true)), variant_name);

class CTEST_CudaNetworkDevice_train : public CTEST_CudaNetworkDevice
{
};

TEST_P(CTEST_CudaNetworkDevice_train, computes_the_cpu_gradients)
{
  Networks nets{GetParam()};
  network& cpu = *nets.cpu->get();
  network& gpu = *nets.gpu->get();
  const std::vector<float> input = input_of(cpu);
  const layer& last = cpu.layers[cpu.n - 1];
  std::vector<float> truth(static_cast<size_t>(
      std::max(get_network_output_size(cpu), last.truths) * cpu.batch));
  std::vector<float> in = input;

  nets.on_cpu([&] { train_network_datum(cpu, in.data(), truth.data()); });

  in = input;
  train_network_datum(gpu, in.data(), truth.data());
  nets.gpu->device().pull(gpu);

  EXPECT_TRUE(close(updates(gpu), updates(cpu)));
}

INSTANTIATE_TEST_SUITE_P(Generated, CTEST_CudaNetworkDevice_train,
                         ValuesIn(computed(true, false)), variant_name);
