#include "Recognition.h"

#include <iomanip>
#include <sstream>

namespace darknetxx011
{

bool Recognition::succeeded() const { return error.empty(); }

std::string Recognition::summary_text() const
{
  if (!succeeded()) {
    return error;
  }

  std::ostringstream text;

  text << objects.size() << (objects.size() == 1 ? " object" : " objects")
       << " recognized in " << std::fixed << std::setprecision(3) << seconds
       << " s";

  return text.str();
}

}  // namespace darknetxx011
