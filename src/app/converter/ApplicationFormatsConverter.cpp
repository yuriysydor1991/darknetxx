#include "src/app/converter/ApplicationFormatsConverter.h"

#include <cassert>
#include <memory>

#include "src/app/ApplicationContext.h"
#include "src/app/converter/ConversionFormats.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/weights-converter/WeightsConverter.h"
#include "src/log/log.h"

namespace app::converter
{

namespace
{

using darknet_adaptor::loaders::WeightsFormat;

WeightsFormat weights_format_of(const ConversionFormat format)
{
  return format == ConversionFormat::dxxwjz1 ? WeightsFormat::Dxxwjz1
                                             : WeightsFormat::Original;
}

}  // namespace

bool ApplicationFormatsConverter::convert(
    std::shared_ptr<ApplicationContext> ctx)
{
  assert(ctx != nullptr);

  if (ctx == nullptr) {
    LOGE("No valid application context provided");
    return false;
  }

  ConversionFormat from{};
  ConversionFormat into{};

  if (!ConversionFormats::parse(ctx->convert_from, from)) {
    LOGE("Unknown source weights format: " << ctx->convert_from);
    return false;
  }

  if (!ConversionFormats::parse(ctx->convert_to, into)) {
    LOGE("Unknown destination weights format: " << ctx->convert_to);
    return false;
  }

  WeightsConverterPtr weights_converter = create_weights_converter();

  assert(weights_converter != nullptr);

  if (weights_converter == nullptr) {
    LOGE("Fail to create the weights files converter");
    return false;
  }

  darknet_adaptor::loaders::WeightsConversion conversion;
  conversion.from = weights_format_of(from);
  conversion.src = ctx->convert_src;
  conversion.to = weights_format_of(into);
  conversion.dst = ctx->convert_dst;
  conversion.cfg = ctx->convert_cfg;
  // the stop signal of the application ends the conversion between its
  // steps, the destination is written by the complete conversion alone
  conversion.stop_requested = [ctx]() { return ctx->stop.load(); };

  return weights_converter->convert(conversion);
}

ApplicationFormatsConverter::WeightsConverterPtr
ApplicationFormatsConverter::create_weights_converter()
{
  return darknet_adaptor::loaders::WeightsConverter::create();
}

}  // namespace app::converter
