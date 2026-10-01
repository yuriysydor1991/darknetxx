#ifndef YOUR_CPP_APP_TEMPLATE_PROJECT_APPLICATIONFORMATSCONVERTER_CLASS_H
#define YOUR_CPP_APP_TEMPLATE_PROJECT_APPLICATIONFORMATSCONVERTER_CLASS_H

#include <memory>

#include "src/app/ApplicationContext.h"
#include "src/app/converter/IFormatsConverter.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/weights-converter/WeightsConverter.h"

namespace app::converter
{

/**
 * @brief Converts a network weights file of one format into another one.
 *
 * The conversion itself is the one of the darknet adaptor (see the
 * darknet_adaptor::loaders::WeightsConverter), which keeps every byte of the
 * file, this class hands it the command line of the context: the formats, the
 * files, the network cfg and the stop request of the application.
 */
class ApplicationFormatsConverter : public IFormatsConverter
{
 public:
  using WeightsConverterPtr = darknet_adaptor::loaders::WeightsConverterPtr;

  virtual ~ApplicationFormatsConverter() = default;
  ApplicationFormatsConverter() = default;

  /**
   * @brief Implemented conversion. See the IFormatsConverter interface.
   *
   * @param ctx A parsed context naming both formats and both paths.
   *
   * @return Returns true when the conversion succeeded and false otherwise,
   * the unknown format names included.
   */
  virtual bool convert(std::shared_ptr<ApplicationContext> ctx) override;

 protected:
  /// @brief The converter of the weights files.
  virtual WeightsConverterPtr create_weights_converter();
};

}  // namespace app::converter

#endif  // YOUR_CPP_APP_TEMPLATE_PROJECT_APPLICATIONFORMATSCONVERTER_CLASS_H
