#ifndef DARKNETXX_PROJECT_NETWORKRECOGNIZER_CLASS_H
#define DARKNETXX_PROJECT_NETWORKRECOGNIZER_CLASS_H

#include <string>

#include "IRecognizer.h"
#include "src/darknet-adaptor/adaptors/ANetworkAdaptor/ANetworkAdaptor.h"
#include "src/recognition/IRecognitionMethod.h"

namespace darknetxx_impl
{

/**
 * @brief Recognizes the images by the inference network of the weights file
 * the way the layers of the network tell: detects the objects in them or
 * classifies them (see the IRecognitionMethod). The network is loaded by the
 * first recognition of it's files and kept for the next ones.
 */
class NetworkRecognizer : public darknetxx011::IRecognizer
{
 public:
  using LibraryContext = darknetxx011::LibraryContext;

  virtual ~NetworkRecognizer() = default;
  NetworkRecognizer() = default;

  virtual darknetxx011::Recognition recognize(
      const LibraryContext& ctx) override;

 protected:
  /// @brief Loads the network of the files of the context, unless it is the
  /// loaded one. False on the failure, the reason of it in the error.
  virtual bool load(const LibraryContext& ctx, std::string& error);

  /// @brief Recognizes the image of the context by the loaded network.
  virtual void recognize_image(const LibraryContext& ctx,
                               darknetxx011::Recognition& recognition);

 private:
  /// @brief The weights, the network and the names files of the network.
  std::string mfiles;
  darknet_adaptor::adaptors::ANetworkAdaptorPtr mnet;
  IRecognitionMethod::ClassNames mnames;
  IRecognitionMethodPtr mmethod;
};

}  // namespace darknetxx_impl

#endif  // DARKNETXX_PROJECT_NETWORKRECOGNIZER_CLASS_H
