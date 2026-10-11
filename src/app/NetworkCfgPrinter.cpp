#include "src/app/NetworkCfgPrinter.h"

#include <cassert>
#include <iostream>
#include <memory>
#include <string>

#include "LibraryFacade.h"
#include "src/log/log.h"

namespace app
{

int NetworkCfgPrinter::run(std::shared_ptr<ApplicationContext> ctx)
{
  assert(ctx != nullptr);

  if (ctx == nullptr) {
    LOGE("No valid application context provided");
    return INVALID;
  }

  const std::string cfg =
      darknetxx011::LibraryFacade::network_cfg(ctx->dump_cfg);

  if (cfg.empty()) {
    LOGE("No network cfg of the "
         << ctx->dump_cfg
         << " file to print, the dxxwjz2, the dxxwjz2p, the dxxwjz3 and the "
            "dxxwjz3p files keep it");
    return INVALID;
  }

  // the cfg alone goes into the standard output, the errors into the stderr
  if (!(std::cout << cfg << std::flush)) {
    LOGE("Fail to print the network cfg of the " << ctx->dump_cfg << " file");
    return INVALID;
  }

  return 0;
}

}  // namespace app
