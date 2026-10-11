#ifndef DARKNETXX_PROJECT_NETWORKCFGPRINTER_CLASS_H
#define DARKNETXX_PROJECT_NETWORKCFGPRINTER_CLASS_H

#include <memory>

#include "src/app/ApplicationContext.h"
#include "src/app/IApplication.h"

namespace app
{

/**
 * @brief The network cfg printer application: prints the darknet (YOLO4) cfg
 * of the network the weights file of the --dump-cfg flag keeps (see the
 * darknetxx011::LibraryFacade::network_cfg) into the standard output, nothing
 * else, so the output redirected into a file is the cfg file of the network
 * to examine or to train anew.
 */
class NetworkCfgPrinter : public IApplication
{
 public:
  virtual ~NetworkCfgPrinter() = default;
  NetworkCfgPrinter() = default;

  /**
   * @brief Prints the network cfg. See the IApplication interface.
   *
   * @param ctx Application's run context with command line parameters etc.
   *
   * @return Returns a zero value once the cfg is printed and other value for
   * the file of no network cfg or unable to be read.
   */
  virtual int run(std::shared_ptr<ApplicationContext> ctx) override;
};

}  // namespace app

#endif  // DARKNETXX_PROJECT_NETWORKCFGPRINTER_CLASS_H
