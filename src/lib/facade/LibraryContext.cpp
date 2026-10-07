#include "LibraryContext.h"

namespace darknetxx011
{

bool LibraryContext::conversion_requested() const
{
  return !convert_from.empty() || !convert_src.empty() || !convert_to.empty() ||
         !convert_dst.empty();
}

void LibraryContext::request_stop() { mstop.store(true); }

void LibraryContext::withdraw_stop() { mstop.store(false); }

bool LibraryContext::stop_requested() const
{
  const std::atomic_bool* const flag = mstopFlag.load();

  return mstop.load() || (flag != nullptr && flag->load());
}

void LibraryContext::stop_with(const std::atomic_bool& flag)
{
  mstopFlag.store(&flag);
}

}  // namespace darknetxx011
