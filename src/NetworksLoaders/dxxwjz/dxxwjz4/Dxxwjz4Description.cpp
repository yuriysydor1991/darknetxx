#include "src/NetworksLoaders/dxxwjz/dxxwjz4/Dxxwjz4Description.h"

#include <string>
#include <utility>

#include "src/NetworksLoaders/dxxwjz/dxxwjz1/Dxxwjz1JsonString.h"
#include "src/NetworksLoaders/dxxwjz/dxxwjz4/Dxxwjz4Format.h"

namespace darknet_adaptor::loaders::dxxwjz4::v4
{

Dxxwjz4Description::Dxxwjz4Description(
    dxxwjz2::v2::Dxxwjz2SubtreeBuilder::Failure nfail)
    : fail{std::move(nfail)}, mbuilder{fail}
{
}

std::string Dxxwjz4Description::member(const std::string& text)
{
  using dxxwjz1::v1::Dxxwjz1JsonString;

  return Dxxwjz1JsonString::quote(Dxxwjz4Format::DESCRIPTION_KEY) + ": " +
         Dxxwjz1JsonString::quote(text);
}

Dxxwjz4Description::Delegate* Dxxwjz4Description::start()
{
  mbuilder.reset(Dxxwjz4Format::DESCRIPTION_KEY);

  return &mbuilder;
}

bool Dxxwjz4Description::read(std::string& description)
{
  const dxxwjz2::v2::Dxxwjz2SubtreeBuilder::json& value = mbuilder.tree();

  if (!value.is_string()) {
    return fail("\"" + std::string{Dxxwjz4Format::DESCRIPTION_KEY} +
                "\" isn't the string of the description text");
  }

  description = value.get<std::string>();

  // the tree of the long text isn't kept
  mbuilder.reset(Dxxwjz4Format::DESCRIPTION_KEY);

  return true;
}

}  // namespace darknet_adaptor::loaders::dxxwjz4::v4
