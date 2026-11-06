#ifndef DARKNET_CXX_PORT_PROJECT_NETWORK_WEIGHTS_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_NETWORK_WEIGHTS_CLASS_H

#include <cstdint>
#include <memory>
#include <string>

#include "src/NetworksLoaders/DataDriversAliases.h"
#include "src/darknet-adaptor/adaptors/ANetworkAdaptor/ANetworkAdaptor.h"
#include "src/darknet.h"

namespace darknet_adaptor::loaders
{

/**
 * @brief The darknet network of the darknetxx-datadrivers library: the view of
 * the library over the layers and the arrays of the darknet network struct
 * (see the dxxdd::INetworkWeights), which the readers of the library load the
 * weights files into and the writers save the files of.
 *
 * The layout of the view is the walk of the darknet network layers the
 * original save_weights_upto() and load_weights_upto() did (see the
 * dxxdd::WeightsLayout), so the library reads and writes the very same bytes
 * the darknet does. The version of the weights files is the one of the
 * darknet sources: the implementation checks it at the compile time.
 *
 * The view keeps the network it is made of: the network struct belongs to
 * the code which made the view, the adaptor made view keeps the adaptor alive
 * (see the NetworkWeightsConverter, which hands the view of the network it
 * makes over to the library).
 */
class NetworkWeights : public dxxdd::INetworkWeights
{
 public:
  using NetworkWeightsPtr = std::shared_ptr<NetworkWeights>;

  ~NetworkWeights() override = default;

  /// @brief The view of the network struct, which outlives it.
  explicit NetworkWeights(network& net);

  /// @brief The view of the network the view keeps alive.
  explicit NetworkWeights(adaptors::ANetworkAdaptorPtr net);

  /// @brief The view of the network of no arrays written by the library: the
  /// structure the documents are read and validated by (see the
  /// IWeightsReader::read()).
  explicit NetworkWeights(const network& net);

  static NetworkWeightsPtr create(adaptors::ANetworkAdaptorPtr net);

  int layers() const override;
  dxxdd::LayerType type(int layer) const override;
  dxxdd::WeightsLayoutEntries layout(
      const dxxdd::WeightsLayoutOptions& options) const override;
  int classes() const override;

  /// @brief The names= key of the cfg of the adaptor made view, empty for the
  /// view of the network struct.
  std::string names() const override;

  uint64_t seen() const override;
  void loaded_seen(uint64_t seen) override;
  uint64_t saved_seen() override;

  /// @brief The arrays of the network in the order of the weights files: the
  /// walk of the darknet network layers the original functions did.
  static dxxdd::WeightsLayoutEntries layout_of(
      const network& net, const dxxdd::WeightsLayoutOptions& options =
                              dxxdd::WeightsLayoutOptions{});

 private:
  adaptors::ANetworkAdaptorPtr madaptor;
  network& mnet;
};

using NetworkWeightsPtr = NetworkWeights::NetworkWeightsPtr;

}  // namespace darknet_adaptor::loaders

#endif  // DARKNET_CXX_PORT_PROJECT_NETWORK_WEIGHTS_CLASS_H
