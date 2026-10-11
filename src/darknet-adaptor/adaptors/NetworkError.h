#ifndef DARKNET_CXX_PORT_PROJECT_NETWORK_ERROR_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_NETWORK_ERROR_CLASS_H

#include <stdexcept>
#include <string>

namespace darknet_adaptor::adaptors
{

/**
 * @brief The failure to build, resize or load a network or a layer: the
 * errors the original darknet C code reported with the error() call, which
 * ended the process.
 *
 * The C++ network infrastructure throws it instead, so the caller decides:
 * the network loaders log it and fail the load, while the darknet C API
 * implementations end the process the way the original functions did.
 */
class NetworkError : public std::runtime_error
{
 public:
  using std::runtime_error::runtime_error;
};

}  // namespace darknet_adaptor::adaptors

#endif  // DARKNET_CXX_PORT_PROJECT_NETWORK_ERROR_CLASS_H
