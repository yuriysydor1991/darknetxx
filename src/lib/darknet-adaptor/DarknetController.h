#ifndef DARKNET_CXX_PORT_PROJECT_APPLICATION_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_APPLICATION_CLASS_H

#include <memory>

#include "src/lib/darknet-adaptor/DarknetContext.h"
#include "src/lib/darknet-adaptor/adaptors/IDarknetAdaptor.h"
#include "src/lib/facade/public/LibraryContext.h"

namespace darknet_adaptor
{

/**
 * @brief The adaptor class for the orig Darknet code.
 */
class DarknetController
{
 public:
  using LibCtxPtr = darknetxx011::LibraryContextPtr;
  using DarknetControllerPtr = std::shared_ptr<DarknetController>;
  using IDarknetAdaptorPtr = adaptors::IDarknetAdaptorPtr;

  virtual ~DarknetController() = default;
  DarknetController() = default;

  static DarknetControllerPtr create();

  /**
   * @brief Perform darkent action.
   *
   * @param ctx Application's run context with command line parameters etc.
   *
   * @return Returns true on the success and false otherwise.
   */
  virtual bool perform(LibCtxPtr ctx);

 protected:
  virtual DarknetContextPtr create_context(LibCtxPtr lctx);
  virtual IDarknetAdaptorPtr create_appropriate_worker(DarknetContextPtr dctx);

  /**
   * @brief The work of the network file of the weights alone (the dxxwjz2
   * one, which keeps it's network): the detection with it's weights, the
   * training of it's network for none. Nullptr for no such file.
   */
  virtual IDarknetAdaptorPtr create_network_file_worker(DarknetContextPtr dctx);

 private:
  /// @brief The network file of the cfg one (the dxxwjz2 one) keeps the
  /// weights the work starts from when no weights file is given.
  static void take_network_file_weights(const DarknetContextPtr& dctx);
};

using DarknetControllerPtr = DarknetController::DarknetControllerPtr;

}  // namespace darknet_adaptor

#endif  // DARKNET_CXX_PORT_PROJECT_APPLICATION_CLASS_H
