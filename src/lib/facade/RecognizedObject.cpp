#include "RecognizedObject.h"

#include <iomanip>
#include <sstream>

namespace darknetxx011
{

bool RecognizedObject::boxed() const { return width > 0 && height > 0; }

std::string RecognizedObject::text() const
{
  std::ostringstream text;

  text << name << ": " << std::fixed << std::setprecision(1)
       << probability * 100.f << '%';

  if (boxed()) {
    text << " (left_x: " << x << " top_y: " << y << " width: " << width
         << " height: " << height << ')';
  }

  return text.str();
}

}  // namespace darknetxx011
