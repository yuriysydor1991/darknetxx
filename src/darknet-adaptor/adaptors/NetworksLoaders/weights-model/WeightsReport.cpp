#include "src/darknet-adaptor/adaptors/NetworksLoaders/weights-model/WeightsReport.h"

#include <string>
#include <vector>

#include "src/log/log.h"

namespace darknet_adaptor::loaders
{

void WeightsReport::error(const std::string& message)
{
  merrors.push_back(message);
}

void WeightsReport::warning(const std::string& message)
{
  mwarnings.push_back(message);
}

void WeightsReport::merge(const WeightsReport& other)
{
  merrors.insert(merrors.end(), other.merrors.begin(), other.merrors.end());
  mwarnings.insert(mwarnings.end(), other.mwarnings.begin(),
                   other.mwarnings.end());
}

bool WeightsReport::ok() const { return merrors.empty(); }

const std::vector<std::string>& WeightsReport::errors() const
{
  return merrors;
}

const std::vector<std::string>& WeightsReport::warnings() const
{
  return mwarnings;
}

void WeightsReport::log(const std::string& subject) const
{
  for (const std::string& message : merrors) {
    LOGE(subject << ": " << message);
  }

  for (const std::string& message : mwarnings) {
    LOGW(subject << ": " << message);
  }
}

std::string WeightsReport::first_error() const
{
  return merrors.empty() ? std::string{} : merrors.front();
}

}  // namespace darknet_adaptor::loaders
