#ifndef DARKNETXX_PROJECT_IRECOGNITIONMETHOD_CLASS_H
#define DARKNETXX_PROJECT_IRECOGNITIONMETHOD_CLASS_H

#include <memory>
#include <string>
#include <vector>

#include "RecognizedObject.h"
#include "network.h"

namespace darknetxx_impl
{

/**
 * @brief The way a network recognizes an image: the network input made of
 * the image and the objects of the network output.
 */
class IRecognitionMethod
{
 public:
  using ClassNames = std::vector<std::string>;
  using Objects = std::vector<darknetxx011::RecognizedObject>;

  virtual ~IRecognitionMethod() = default;
  IRecognitionMethod() = default;

  /// @brief The objects of the image the network recognizes, the ones more
  /// probable than the threshold, in no particular order.
  virtual Objects recognize(network& net, const image& im, float thresh,
                            const ClassNames& names) const = 0;

 protected:
  /// @brief The name of the class of the index, "class <index>" for none.
  static std::string class_name(const ClassNames& names, const int index)
  {
    return index < static_cast<int>(names.size())
               ? names[static_cast<std::size_t>(index)]
               : "class " + std::to_string(index);
  }
};

using IRecognitionMethodPtr = std::shared_ptr<IRecognitionMethod>;

}  // namespace darknetxx_impl

#endif  // DARKNETXX_PROJECT_IRECOGNITIONMETHOD_CLASS_H
