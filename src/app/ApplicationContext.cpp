#include "src/app/ApplicationContext.h"

namespace app
{

ApplicationContext::ApplicationContext(int& gargc, char**& gargv)
    : argc{gargc}, argv{gargv}
{
}

void ApplicationContext::push_error(const std::string& errorDescription)
{
  errors.emplace_back(errorDescription);
}

bool ApplicationContext::chart_requested() const { return !draw_chart.empty(); }

bool ApplicationContext::conversion_requested() const
{
  return !convert_from.empty() || !convert_src.empty() || !convert_to.empty() ||
         !convert_dst.empty();
}

}  // namespace app
