#ifndef DARKNET_CXX_PORT_PROJECT_WEIGHTS_FILE_FORMATS_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_WEIGHTS_FILE_FORMATS_CLASS_H

#include <memory>
#include <string>
#include <utility>

#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/IWeightsFileFormat.h"

namespace darknet_adaptor::loaders
{

/// @brief The formats of the names alone.
class WeightsFileFormats
{
 public:
  static IWeightsFileFormatPtr named(const std::string& name)
  {
    for (const char* const known : {"weights", "dxxwjz1", "dxxwjz2"}) {
      if (name == known) {
        return std::make_shared<Named>(name);
      }
    }

    return nullptr;
  }

 private:
  class Named : public IWeightsFileFormat
  {
   public:
    explicit Named(std::string nname) : mname{std::move(nname)} {}

    const std::string& name() const override { return mname; }

   private:
    std::string mname;
  };
};

}  // namespace darknet_adaptor::loaders

#endif  // DARKNET_CXX_PORT_PROJECT_WEIGHTS_FILE_FORMATS_CLASS_H
