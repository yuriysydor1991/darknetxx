#include "src/lib/libmain/WeightsConversionLib.h"

#include <cassert>

#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/weights-formats/WeightsFileFormats.h"
#include "src/log/log.h"

namespace darknetxx_impl
{

using darknet_adaptor::loaders::IWeightsFileFormatPtr;
using darknet_adaptor::loaders::WeightsFileFormats;

bool WeightsConversionLib::libcall(LibraryContextPtr ctx)
{
  assert(ctx != nullptr);

  if (ctx == nullptr) {
    LOGE("No valid library context provided");
    return false;
  }

  // the network to train of the cfg has no source to read
  const bool sourced = !ctx->convert_from.empty() || !ctx->convert_src.empty();
  const IWeightsFileFormatPtr from =
      sourced ? WeightsFileFormats::named(ctx->convert_from,
                                          WeightsFileFormats::convertible())
              : nullptr;
  const IWeightsFileFormatPtr into = WeightsFileFormats::named(
      ctx->convert_to, WeightsFileFormats::convertible());

  if (sourced && from == nullptr) {
    LOGE("Unknown source weights format: " << ctx->convert_from);
    return false;
  }

  if (into == nullptr) {
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
  conversion.from = from;
  conversion.src = ctx->convert_src;
  conversion.to = into;
  conversion.dst = ctx->convert_dst;
  conversion.cfg = ctx->convert_cfg;
  conversion.names = ctx->convert_names;
  // the stop request ends the conversion between it's steps, the destination
  // is written by the complete conversion alone
  conversion.stop_requested = [ctx]() { return ctx->stop_requested(); };

  return weights_converter->convert(conversion);
}

WeightsConversionLib::WeightsConverterPtr
WeightsConversionLib::create_weights_converter()
{
  return darknet_adaptor::loaders::WeightsConverter::create();
}

}  // namespace darknetxx_impl
