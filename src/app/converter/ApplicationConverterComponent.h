#ifndef YOUR_CPP_APP_TEMPLATE_PROJECT_APPLICATIONCONVERTERCOMPONENT_CLASS_H
#define YOUR_CPP_APP_TEMPLATE_PROJECT_APPLICATIONCONVERTERCOMPONENT_CLASS_H

#include <memory>

#include "src/app/ApplicationContext.h"
#include "src/app/IApplication.h"
#include "src/app/converter/IFormatsConverter.h"

namespace app::converter
{

/**
 * @brief The application component which runs a requested network weights
 * file conversion.
 *
 * It is the app::Application of the conversion command line flags: the
 * converter is run in place of the training or the detection, which this
 * component never starts. The user is told what is being converted through
 * the log messages alone.
 */
class ApplicationConverterComponent : public IApplication
{
 public:
  virtual ~ApplicationConverterComponent() = default;

  /**
   * @brief An ApplicationConverterComponent constructor.
   *
   * @param nconverter The converter to run.
   */
  explicit ApplicationConverterComponent(
      const IFormatsConverterPtr& nconverter);

  /**
   * @brief Implemented conversion application. See IApplication interface.
   *
   * @param ctx Application's run context with the parsed conversion flags.
   *
   * @return Returns the application return status: a zero value on the
   * success, the 128 + signal number on a stop request of the signal and
   * other value otherwise.
   */
  virtual int run(std::shared_ptr<ApplicationContext> ctx) override;

 private:
  IFormatsConverterPtr converter;
};

}  // namespace app::converter

#endif  // YOUR_CPP_APP_TEMPLATE_PROJECT_APPLICATIONCONVERTERCOMPONENT_CLASS_H
