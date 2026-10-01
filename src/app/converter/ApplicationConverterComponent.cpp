#include "src/app/converter/ApplicationConverterComponent.h"

#include <cassert>
#include <memory>

#include "src/app/ApplicationContext.h"
#include "src/app/converter/IFormatsConverter.h"
#include "src/log/log.h"

namespace app::converter
{

ApplicationConverterComponent::ApplicationConverterComponent(
    const IFormatsConverterPtr& nconverter)
    : converter{nconverter}
{
}

int ApplicationConverterComponent::run(std::shared_ptr<ApplicationContext> ctx)
{
  // the exit status a shell reports for a process the signal killed
  static constexpr const int SIGNAL_EXIT_STATUS_BASE = 128;

  assert(ctx != nullptr);
  assert(converter != nullptr);

  if (ctx == nullptr) {
    LOGE("No valid context pointer provided");
    return INVALID;
  }

  if (converter == nullptr) {
    LOGE("No valid formats converter pointer provided");
    return INVALID;
  }

  LOGI(
      "Converting the network weights file, no training nor detection is "
      "started");

  const bool converted = converter->convert(ctx);

  // the conversion was cut short by a stop request, which is not a success
  if (ctx->stop.load()) {
    const int signalNumber = ctx->stop_signal.load();

    LOGW("The conversion is stopped by the signal " << signalNumber);

    return signalNumber > 0 ? SIGNAL_EXIT_STATUS_BASE + signalNumber : INVALID;
  }

  if (!converted) {
    LOGE("The formats converter returned invalid status");
    return INVALID;
  }

  return 0;
}

}  // namespace app::converter
