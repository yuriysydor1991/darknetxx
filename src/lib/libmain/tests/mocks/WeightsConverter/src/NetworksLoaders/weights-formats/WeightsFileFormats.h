#ifndef DARKNET_CXX_PORT_PROJECT_WEIGHTS_FILE_FORMATS_CLASS_H
#define DARKNET_CXX_PORT_PROJECT_WEIGHTS_FILE_FORMATS_CLASS_H

#include <initializer_list>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "src/NetworksLoaders/IWeightsFileFormat.h"

namespace darknet_adaptor::loaders
{

/// @brief The formats of the names alone.
class WeightsFileFormats
{
 public:
  static const std::vector<IWeightsFileFormatPtr>& all()
  {
    static const std::vector<IWeightsFileFormatPtr> formats =
        of({"weights", "dxxwjz1", "dxxwjz2", "dxxwjz2p"});

    return formats;
  }

  static const std::vector<IWeightsFileFormatPtr>& convertible()
  {
    static const std::vector<IWeightsFileFormatPtr> formats =
        of({"weights", "dxxwjz1", "dxxwjz2", "dxxwjz2p", "yolo4"});

    return formats;
  }

  static IWeightsFileFormatPtr named(
      const std::string& name,
      const std::vector<IWeightsFileFormatPtr>& formats = all())
  {
    for (const IWeightsFileFormatPtr& format : formats) {
      if (format->name() == name) {
        return format;
      }
    }

    return nullptr;
  }

 private:
  static std::vector<IWeightsFileFormatPtr> of(
      std::initializer_list<const char*> names)
  {
    std::vector<IWeightsFileFormatPtr> formats;

    for (const char* const name : names) {
      formats.push_back(std::make_shared<Named>(name));
    }

    return formats;
  }

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
