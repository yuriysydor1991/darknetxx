#ifndef DARKNETXX_PROJECT_WEIGHTSCONVERSIONLIB_CLASS_H
#define DARKNETXX_PROJECT_WEIGHTSCONVERSIONLIB_CLASS_H

#include "ILib.h"
#include "LibraryContext.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/weights-converter/WeightsConverter.h"

namespace darknetxx_impl
{

/**
 * @brief The network weights file conversion the convert_* parameters of the
 * context name (see the WeightsFileFormats).
 */
class WeightsConversionLib : public darknetxx011::ILib
{
 public:
  using LibraryContextPtr = darknetxx011::LibraryContextPtr;
  using WeightsConverterPtr = darknet_adaptor::loaders::WeightsConverterPtr;

  virtual ~WeightsConversionLib() = default;
  WeightsConversionLib() = default;

  virtual bool libcall(LibraryContextPtr ctx) override;

 protected:
  virtual WeightsConverterPtr create_weights_converter();
};

}  // namespace darknetxx_impl

#endif  // DARKNETXX_PROJECT_WEIGHTSCONVERSIONLIB_CLASS_H
