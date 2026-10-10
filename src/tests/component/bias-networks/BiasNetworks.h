#ifndef DARKNETXX_PROJECT_TESTS_BIAS_NETWORKS_H
#define DARKNETXX_PROJECT_TESTS_BIAS_NETWORKS_H

#include <cstdint>
#include <fstream>
#include <string>
#include <vector>

/**
 * @brief The networks of no features computed, the tests of the recognitions
 * share: their outputs are the biases of their single layer of the weights,
 * all of them zero, so they recognize every image the same way. The detector
 * boxes the whole image as the class "b", the classifier tells the image is of
 * the class "c", the 5 of it's bias over the 0 of the others the softmax makes
 * the 98.7% of.
 */
namespace bias_networks
{

inline const char* const DETECTOR_CFG = R"([net]
width=32
height=32
channels=3

[maxpool]
size=32
stride=32

[convolutional]
filters=7
size=1
stride=1
activation=linear

[yolo]
mask=0
anchors=32,32
classes=2
num=1
)";

/// @brief The x, the y, the w, the h, the objectness, the a and the b ones.
inline const std::vector<float> DETECTOR_BIASES{0, 0, 0, 0, 10, -10, 10};
inline const std::size_t DETECTOR_WEIGHTS = 7 * 3;

inline const char* const CLASSIFIER_CFG = R"([net]
width=8
height=8
channels=3

[connected]
output=3
activation=linear

[softmax]
)";

inline const std::vector<float> CLASSIFIER_BIASES{0, 0, 5};
inline const std::size_t CLASSIFIER_WEIGHTS = 8 * 8 * 3 * 3;

/// @brief Writes the cfg file and the original darknet weights file of the
/// biases and the zero weights of the network.
inline void write(const std::string& cfgFile, const std::string& weightsFile,
                  const std::string& cfg, const std::vector<float>& biases,
                  const std::size_t weights)
{
  const int32_t version[] = {0, 2, 0};
  const uint64_t seen = 0U;
  const std::vector<float> zeros(weights);
  std::ofstream out{weightsFile, std::ios::binary};

  out.write(reinterpret_cast<const char*>(version), sizeof(version));
  out.write(reinterpret_cast<const char*>(&seen), sizeof(seen));
  out.write(reinterpret_cast<const char*>(biases.data()),
            static_cast<std::streamsize>(biases.size() * sizeof(float)));
  out.write(reinterpret_cast<const char*>(zeros.data()),
            static_cast<std::streamsize>(zeros.size() * sizeof(float)));

  std::ofstream{cfgFile} << cfg;
}

/// @brief Writes the 32 x 24 image of the 160, 80, 40 RGB color, the PPM one.
inline void write_image(const std::string& file)
{
  std::ofstream out{file, std::ios::binary};

  out << "P6\n32 24\n255\n";

  for (int pixel = 0; pixel < 32 * 24; ++pixel) {
    out.put(static_cast<char>(160)).put(80).put(40);
  }
}

}  // namespace bias_networks

#endif  // DARKNETXX_PROJECT_TESTS_BIAS_NETWORKS_H
