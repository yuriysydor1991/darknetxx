#ifndef DARKNET_CXX_PORT_PROJECT_DATA_DRIVERS_ALIASES_H
#define DARKNET_CXX_PORT_PROJECT_DATA_DRIVERS_ALIASES_H

#include <functional>
#include <initializer_list>
#include <memory>
#include <string>
#include <utility>
#include <vector>

/**
 * @file
 *
 * The mock of the darknetxx-datadrivers library the WeightsConversionLib
 * talks to: the formats of the names alone, the conversion of the fields
 * alone and the converter interface the mock converter implements (see the
 * network-weights/NetworkWeightsConverter.h mock beside).
 */
namespace DarknetxxDataDrivers012
{

class IWeightsFileFormat
{
 public:
  virtual ~IWeightsFileFormat() = default;

  virtual const std::string& name() const = 0;
};

using IWeightsFileFormatPtr = std::shared_ptr<const IWeightsFileFormat>;

struct WeightsConversion
{
  IWeightsFileFormatPtr from;
  std::string src;
  IWeightsFileFormatPtr to;
  std::string dst;
  std::string cfg;
  std::string names;
  std::string description;
  std::function<bool()> stop_requested;
};

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

/// @brief The converter interface of the library.
class WeightsConverter
{
 public:
  using WeightsConverterPtr = std::shared_ptr<WeightsConverter>;

  virtual ~WeightsConverter() = default;

  virtual bool convert(const WeightsConversion& conversion) = 0;
};

using WeightsConverterPtr = WeightsConverter::WeightsConverterPtr;

}  // namespace DarknetxxDataDrivers012

namespace dxxdd = DarknetxxDataDrivers012;

#endif  // DARKNET_CXX_PORT_PROJECT_DATA_DRIVERS_ALIASES_H
