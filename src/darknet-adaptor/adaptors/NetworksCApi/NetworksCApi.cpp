/**
 * @brief The darknet C API of the network memory and the weights files,
 * implemented by the C++ network infrastructure.
 *
 * The darknet C code (the training loop and the rest of the original darknet
 * functions) keeps calling the functions it always did: parse_network_cfg(),
 * load_weights(), save_weights(), resize_network(), free_network() and the
 * like. Their implementations live here now, on top of the C++ adaptors: the
 * networks are made by the OrigDefaultLoader, their memory belongs to the
 * ANetworkAdaptor and ALayerAdaptor containers, the weights files are read and
 * written by the readers and the writers of the formats of their names (see
 * the WeightsFileFormats). The C code never allocates nor frees the network
 * memory itself.
 *
 * The networks these functions make for the C code are kept alive by the
 * NetworksRegistry until the free_network() of their view. The errors end the
 * process the way the original functions did.
 */

#include <cstring>
#include <exception>
#include <new>
#include <string>

#include "src/darknet-adaptor/adaptors/ANetworkAdaptor/ANetworkAdaptor.h"
#include "src/darknet-adaptor/adaptors/NetworkError.h"
#include "src/darknet-adaptor/adaptors/NetworksCApi/NetworksRegistry.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/OrigDefaultLoader/OrigDefaultLoader.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/weights-formats/WeightsFileFormats.h"
#include "src/darknet-adaptor/adaptors/orig-includes.h"

namespace
{

using darknet_adaptor::adaptors::ANetworkAdaptorPtr;
using darknet_adaptor::adaptors::NetworksRegistry;
using darknet_adaptor::loaders::OrigDefaultLoader;
using darknet_adaptor::loaders::WeightsFileFormats;

std::string path_of(const char* path) { return path ? std::string{path} : ""; }

/// @brief Ends the process with the error, the way the original did.
[[noreturn]] void fail(const char* function, const std::exception& failure)
{
  CLOGE("%s: %s", function, failure.what());
  error("The network infrastructure failure", DARKNET_LOC);
  // the error() exits, the abort() keeps the compiler aware of it
  std::abort();
}

/// @brief Registers the network the C code gets and returns it's view.
network hand_over(const ANetworkAdaptorPtr& net)
{
  NetworksRegistry::add(net, true);
  return *net->get();
}

/**
 * @brief The adaptor of the network of the view, the view scalar fields
 * taken over by the adaptor struct: the C code changes the scalars of it's
 * copies (the batch of the dynamic mini batch and the like), never their
 * buffers.
 */
ANetworkAdaptorPtr adaptor_of(const network& view, const char* function)
{
  ANetworkAdaptorPtr net = NetworksRegistry::find(view);

  if (!net) {
    CLOGE("%s: the network isn't made by the network infrastructure", function);
    error("Unknown network", DARKNET_LOC);
    std::abort();
  }

  network& own = *net->get();

  if (own.seen != view.seen || own.cur_iteration != view.cur_iteration) {
    CLOGE("%s: the network view doesn't match it's network", function);
    error("Invalid network view", DARKNET_LOC);
    std::abort();
  }

  // the workspace of an outdated copy, which the C code replaces with the
  // written back one, is not taken over
  float* const workspace = own.workspace;
  own = view;
  own.workspace = workspace;

  return net;
}

}  // namespace

extern "C" {

network parse_network_cfg(char* filename)
{
  return parse_network_cfg_custom(filename, 0, 0);
}

network parse_network_cfg_custom(char* filename, int batch, int time_steps)
{
  try {
    OrigDefaultLoader loader;
    return hand_over(
        loader.parse_network_cfg_custom(path_of(filename), batch, time_steps));
  }
  catch (const std::exception& failure) {
    fail(__func__, failure);
  }
}

network* load_network_custom(char* cfg, char* weights, int clear, int batch)
{
  try {
    OrigDefaultLoader loader;
    const ANetworkAdaptorPtr net = loader.load_network_custom(
        path_of(cfg), path_of(weights), clear != 0, batch);

    // the struct the caller releases with the free() after the
    // free_network() of it
    network* holder = static_cast<network*>(xcalloc(1, sizeof(network)));
    *holder = hand_over(net);
    return holder;
  }
  catch (const std::exception& failure) {
    fail(__func__, failure);
  }
}

network* load_network(char* cfg, char* weights, int clear)
{
  if (cfg == NULL) {
    CLOGEM("Invalid config file pointer provided");
    return NULL;
  }

  if (std::strlen(cfg) == 0) {
    CLOGEM("Invalid config file path string provided");
    return NULL;
  }

  try {
    OrigDefaultLoader loader;
    const ANetworkAdaptorPtr net =
        loader.load_network(path_of(cfg), path_of(weights), clear != 0);

    // the struct the caller releases with the free() after the
    // free_network() of it
    network* holder = static_cast<network*>(xcalloc(1, sizeof(network)));
    *holder = hand_over(net);
    return holder;
  }
  catch (const std::exception& failure) {
    fail(__func__, failure);
  }
}

void load_weights_upto(network* net, char* filename, int cutoff)
{
  const std::string path = path_of(filename);

  if (!WeightsFileFormats::of_file(path)->create_reader()->load(*net, path,
                                                                cutoff)) {
    file_error(__FILE__, __LINE__, filename);
  }
}

void load_weights(network* net, char* filename)
{
  load_weights_upto(net, filename, net->n);
}

void save_weights_upto(network net, char* filename, int cutoff, int save_ema)
{
  const std::string path = path_of(filename);

  if (!WeightsFileFormats::of_file(path)->create_writer()->save(
          net, path, cutoff, save_ema != 0)) {
    file_error(__FILE__, __LINE__, filename);
  }
}

void save_weights(network net, char* filename)
{
  save_weights_upto(net, filename, net.n, 0);
}

void free_network(network net)
{
  if (NetworksRegistry::owns(net)) {
    NetworksRegistry::remove(net);
    return;
  }

  if (NetworksRegistry::find(net)) {
    // the C++ owner releases it
    CLOGEM("The free_network() of a network the C++ owns, left to the owner");
    return;
  }

  if (net.layers != NULL) {
    CLOGEM("The free_network() of an unknown network, nothing to free");
  }
}

void free_network_ptr(network* net) { free_network(*net); }

int resize_network(network* net, int w, int h)
{
  const ANetworkAdaptorPtr adaptor = adaptor_of(*net, __func__);

  try {
    adaptor->resize(w, h);
  }
  catch (const std::exception& failure) {
    fail(__func__, failure);
  }

  *net = *adaptor->get();

  return 0;
}

void set_batch_network(network* net, int b)
{
  const ANetworkAdaptorPtr adaptor = adaptor_of(*net, __func__);

  try {
    adaptor->set_batch(b);
  }
  catch (const std::exception& failure) {
    fail(__func__, failure);
  }

  *net = *adaptor->get();
}

void fuse_conv_batchnorm(network net)
{
  const ANetworkAdaptorPtr adaptor = adaptor_of(net, __func__);

  try {
    adaptor->fuse_conv_batchnorm();
  }
  catch (const std::exception& failure) {
    fail(__func__, failure);
  }
}

void calculate_binary_weights(network net)
{
  const ANetworkAdaptorPtr adaptor = adaptor_of(net, __func__);

  try {
    adaptor->calculate_binary_weights();
  }
  catch (const std::exception& failure) {
    fail(__func__, failure);
  }
}

void copy_weights_net(network net_train, network* net_map)
{
  darknet_adaptor::adaptors::ANetworkAdaptor::copy_weights_net(net_train,
                                                               *net_map);
}

}  // extern "C"
