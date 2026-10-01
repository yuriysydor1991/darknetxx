#ifndef YOUR_CPP_APP_TEMPLATE_PROJECT_IFORMATSCONVERTER_ABSTRACT_CLASS_H
#define YOUR_CPP_APP_TEMPLATE_PROJECT_IFORMATSCONVERTER_ABSTRACT_CLASS_H

#include <memory>

#include "src/app/ApplicationContext.h"

namespace app::converter
{

/**
 * @brief Class interface to be implemented by the network weights files
 * format converters.
 *
 * The application component which runs a conversion depends on this interface
 * and not on a particular converter.
 */
class IFormatsConverter
{
 public:
  using IFormatsConverterPtr = std::shared_ptr<IFormatsConverter>;

  virtual ~IFormatsConverter() = default;
  IFormatsConverter() = default;

  /**
   * @brief Reads the weights file of the source format and writes it in the
   * destination format.
   *
   * @param ctx A parsed context naming both formats and both paths.
   *
   * @return Implementors should return true when the conversion succeeded
   * and false otherwise.
   */
  virtual bool convert(std::shared_ptr<ApplicationContext> ctx) = 0;
};

using IFormatsConverterPtr = IFormatsConverter::IFormatsConverterPtr;

}  // namespace app::converter

#endif  // YOUR_CPP_APP_TEMPLATE_PROJECT_IFORMATSCONVERTER_ABSTRACT_CLASS_H
