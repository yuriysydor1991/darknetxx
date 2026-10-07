#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz2/v2/Dxxwjz2JsonEmitter.h"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <ostream>
#include <string>
#include <vector>

#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz1/v1/Dxxwjz1Format.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz1/v1/Dxxwjz1JsonString.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz2/v2/Dxxwjz2Format.h"

namespace darknet_adaptor::loaders::dxxwjz2::v2
{

using dxxwjz1::v1::Dxxwjz1Format;
using dxxwjz1::v1::Dxxwjz1JsonString;

Dxxwjz2JsonEmitter::Dxxwjz2JsonEmitter(std::ostream& out,
                                       const NetworkDescription& network,
                                       const TrainingRecord& training)
    : Dxxwjz1JsonEmitter{out}, mnetwork{network}, mtraining{training}
{
}

const char* Dxxwjz2JsonEmitter::format_name() const
{
  return Dxxwjz2Format::NAME;
}

int64_t Dxxwjz2JsonEmitter::format_version() const
{
  return Dxxwjz2Format::VERSION;
}

void Dxxwjz2JsonEmitter::sections()
{
  network();
  training();
}

void Dxxwjz2JsonEmitter::network()
{
  const std::vector<std::string>& classes = mnetwork.classes();

  write(",\n  " + key(Dxxwjz2Format::NETWORK_KEY) + "{" +
        key(Dxxwjz2Format::CFG_KEY) + Dxxwjz1JsonString::quote(mnetwork.cfg()) +
        ", " + key(Dxxwjz2Format::CLASSES_KEY) + "[");

  for (std::size_t index = 0U; index < classes.size(); ++index) {
    write(std::string{index == 0U ? "\n    {" : ",\n    {"} +
          key(Dxxwjz1Format::INDEX_KEY) + std::to_string(index) + ", " +
          key(Dxxwjz2Format::NAME_KEY) +
          Dxxwjz1JsonString::quote(classes[index]) + "}");
  }

  write(classes.empty() ? "]}" : "\n  ]}");
}

void Dxxwjz2JsonEmitter::training()
{
  const double time = mtraining.seconds();
  const long long seconds =
      std::isfinite(time) ? std::llround(std::max(0.0, time)) : 0LL;

  write(",\n  " + key(Dxxwjz2Format::TRAINING_KEY) + "{" +
        key(Dxxwjz2Format::SECONDS_KEY) + std::to_string(seconds) + ",\n    " +
        key(Dxxwjz2Format::ITERATION_KEY));
  integers(mtraining.iterations());

  write(",\n    " + key(Dxxwjz2Format::LOSS_KEY));
  values(mtraining.losses().data(), mtraining.size());

  write(",\n    " + key(Dxxwjz2Format::AVG_LOSS_KEY));
  values(mtraining.avg_losses().data(), mtraining.size());

  // the trainings of no mAP calculation write the files of the losses alone
  if (mtraining.map_size() > 0U) {
    write(",\n    " + key(Dxxwjz2Format::MAP_ITERATION_KEY));
    integers(mtraining.map_iterations());

    write(",\n    " + key(Dxxwjz2Format::MAP_KEY));
    values(mtraining.maps().data(), mtraining.map_size());
  }

  write("\n  }");
}

void Dxxwjz2JsonEmitter::integers(const std::vector<int>& values)
{
  write("[", 1U);

  for (std::size_t index = 0U; index < values.size(); ++index) {
    write((index == 0U ? "" : ",") + std::to_string(values[index]));
  }

  write("]", 1U);
}

}  // namespace darknet_adaptor::loaders::dxxwjz2::v2
