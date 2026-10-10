#ifndef DARKNETXX_011_IRECOGNIZER_CLASS_H
#define DARKNETXX_011_IRECOGNIZER_CLASS_H

#include <memory>

#include "LibraryAPI.h"
#include "LibraryContext.h"
#include "Recognition.h"

namespace darknetxx011
{

/**
 * @brief Recognizes the images by the network of a weights file: detects the
 * objects in them or classifies the whole of them, the way the network does.
 * One recognition at a time.
 */
class DARKNETXX_LIB_API IRecognizer
{
 public:
  virtual ~IRecognizer() = default;
  IRecognizer() = default;

  /**
   * @brief Recognizes the image file of the context (the image) by the
   * network of the weights file of any format (the latest_weights), of the
   * network cfg file (the detect_cfg) unless the weights file keeps it's
   * network and of the class names file (the names) unless the network keeps
   * them. The objects are the ones more probable than the threshold (the
   * thresh). The network stays loaded for the next images of the same files.
   */
  virtual Recognition recognize(const LibraryContext& ctx) = 0;
};

using IRecognizerPtr = std::shared_ptr<IRecognizer>;

}  // namespace darknetxx011

#endif  // DARKNETXX_011_IRECOGNIZER_CLASS_H
