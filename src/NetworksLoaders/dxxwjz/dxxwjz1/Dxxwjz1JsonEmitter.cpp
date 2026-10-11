#include "src/NetworksLoaders/dxxwjz/dxxwjz1/Dxxwjz1JsonEmitter.h"

#include <cstddef>
#include <cstring>
#include <ostream>
#include <string>
#include <vector>

#include "project-global-decls.h"
#include "src/NetworksLoaders/dxxwjz/dxxwjz1/Dxxwjz1Base64.h"
#include "src/NetworksLoaders/dxxwjz/dxxwjz1/Dxxwjz1FloatCodec.h"
#include "src/NetworksLoaders/dxxwjz/dxxwjz1/Dxxwjz1Format.h"
#include "src/NetworksLoaders/dxxwjz/dxxwjz1/Dxxwjz1JsonString.h"

namespace darknet_adaptor::loaders::dxxwjz1::v1
{

namespace
{

/// @brief The size of the text chunk handed over to the stream.
constexpr const std::size_t CHUNK = 64U * 1024U;

}  // namespace

Dxxwjz1JsonEmitter::Dxxwjz1JsonEmitter(std::ostream& out)
    : mout{out}, mbuffer(CHUNK)
{
}

std::string Dxxwjz1JsonEmitter::generator()
{
  return project_decls::PROJECT_NAME + " " +
         project_decls::PROJECT_BUILD_VERSION;
}

const char* Dxxwjz1JsonEmitter::format_name() const
{
  return Dxxwjz1Format::NAME;
}

int64_t Dxxwjz1JsonEmitter::format_version() const
{
  return Dxxwjz1Format::VERSION;
}

void Dxxwjz1JsonEmitter::sections() {}

std::string Dxxwjz1JsonEmitter::trailing(
    const std::vector<uint8_t>& bytes) const
{
  return Dxxwjz1JsonString::quote(Dxxwjz1Base64::encode(bytes));
}

std::string Dxxwjz1JsonEmitter::key(const char* const name)
{
  return Dxxwjz1JsonString::quote(name) + ": ";
}

void Dxxwjz1JsonEmitter::begin(const WeightsHeader& header, const bool weights)
{
  write("{\n  " + key(Dxxwjz1Format::FORMAT_KEY) +
        Dxxwjz1JsonString::quote(format_name()) + ",\n  " +
        key(Dxxwjz1Format::VERSION_KEY) + std::to_string(format_version()) +
        ",\n  " + key(Dxxwjz1Format::GENERATOR_KEY) +
        Dxxwjz1JsonString::quote(generator()));

  sections();

  mfirst_layer = true;
  mweights = weights;

  if (!weights) {
    return;
  }

  write(",\n  " + header_member(header) + ",\n  " +
        key(Dxxwjz1Format::LAYERS_KEY) + "[");
}

std::string Dxxwjz1JsonEmitter::header_member(const WeightsHeader& header)
{
  return key(Dxxwjz1Format::HEADER_KEY) + "{" + key(Dxxwjz1Format::MAJOR_KEY) +
         std::to_string(header.major) + ", " + key(Dxxwjz1Format::MINOR_KEY) +
         std::to_string(header.minor) + ", " +
         key(Dxxwjz1Format::REVISION_KEY) + std::to_string(header.revision) +
         ", " + key(Dxxwjz1Format::SEEN_KEY) + std::to_string(header.seen) +
         "}";
}

void Dxxwjz1JsonEmitter::layer(const int index, const LAYER_TYPE type,
                               const std::vector<Dxxwjz1ArrayView>& arrays)
{
  write(mfirst_layer ? "\n    {" : ",\n    {");
  mfirst_layer = false;

  write(key(Dxxwjz1Format::INDEX_KEY) + std::to_string(index) + ", " +
        key(Dxxwjz1Format::TYPE_KEY) +
        Dxxwjz1JsonString::quote(WeightsSchema::name(type)));
  this->arrays(arrays);
}

void Dxxwjz1JsonEmitter::arrays(const std::vector<Dxxwjz1ArrayView>& arrays)
{
  if (arrays.empty()) {
    write("}");
    return;
  }

  const bool nested = arrays.front().sub != WeightsSubLayer::None;
  // the indent of the arrays: the ones of the layer, the ones of the
  // sub-layers
  const std::string indent = nested ? "\n        " : "\n      ";

  write(nested ? ", " + key(Dxxwjz1Format::SUBLAYERS_KEY) + "{"
               : ", " + key(Dxxwjz1Format::ARRAYS_KEY) + "{");

  for (std::size_t position = 0U; position < arrays.size(); ++position) {
    const Dxxwjz1ArrayView& array = arrays[position];
    const bool first_of_sub =
        position == 0U || arrays[position - 1U].sub != array.sub;

    if (nested && first_of_sub) {
      write(std::string{position == 0U ? "" : "\n      }},"} + "\n      " +
            key(WeightsSchema::name(array.sub).c_str()) + "{" +
            key(Dxxwjz1Format::ARRAYS_KEY) + "{");
    } else if (!first_of_sub || !nested) {
      write(position == 0U ? "" : ",");
    }

    write(indent + key(WeightsSchema::name(array.kind).c_str()));
    values(array.data, array.count);
  }

  write(nested ? "\n      }}\n    }}" : "\n    }}");
}

void Dxxwjz1JsonEmitter::end(const std::vector<uint8_t>& bytes)
{
  if (mweights) {
    write(mfirst_layer ? "]" : "\n  ]");
  }

  if (mweights && !bytes.empty()) {
    write(",\n  " + key(Dxxwjz1Format::TRAILING_KEY) + trailing(bytes));
  }

  write("\n}\n");

  flush();
}

void Dxxwjz1JsonEmitter::values(const float* data, const std::size_t count)
{
  write("[", 1U);

  for (std::size_t index = 0U; index < count; ++index) {
    if (mused + Dxxwjz1FloatCodec::MAX_TEXT + 1U > mbuffer.size()) {
      flush();
    }

    char* const first = mbuffer.data() + mused;
    char* last = first;

    if (index > 0U) {
      *last++ = ',';
    }

    last = Dxxwjz1FloatCodec::encode(data == nullptr ? 0.f : data[index], last);

    mused += static_cast<std::size_t>(last - first);
  }

  write("]", 1U);
}

void Dxxwjz1JsonEmitter::write(const char* text, const std::size_t size)
{
  if (mused + size > mbuffer.size()) {
    flush();
  }

  if (size > mbuffer.size()) {
    mout.write(text, static_cast<std::streamsize>(size));
    return;
  }

  std::memcpy(mbuffer.data() + mused, text, size);
  mused += size;
}

void Dxxwjz1JsonEmitter::write(const std::string& text)
{
  write(text.data(), text.size());
}

void Dxxwjz1JsonEmitter::flush()
{
  if (mused > 0U) {
    mout.write(mbuffer.data(), static_cast<std::streamsize>(mused));
    mused = 0U;
  }
}

}  // namespace darknet_adaptor::loaders::dxxwjz1::v1
