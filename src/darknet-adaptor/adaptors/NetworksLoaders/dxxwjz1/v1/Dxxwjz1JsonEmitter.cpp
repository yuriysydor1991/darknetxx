#include "src/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz1/v1/Dxxwjz1JsonEmitter.h"

#include <cstddef>
#include <cstring>
#include <ostream>
#include <string>
#include <vector>

#include "project-global-decls.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz1/v1/Dxxwjz1Base64.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz1/v1/Dxxwjz1FloatCodec.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz1/v1/Dxxwjz1Format.h"

namespace darknet_adaptor::loaders::dxxwjz1::v1
{

namespace
{

/// @brief The size of the text chunk handed over to the stream.
constexpr const std::size_t CHUNK = 64U * 1024U;

/// @brief The JSON string of the text of the ASCII names.
std::string quoted(const std::string& text)
{
  std::string result{"\""};

  for (const char symbol : text) {
    if (symbol == '"' || symbol == '\\') {
      result.push_back('\\');
      result.push_back(symbol);
    } else if (static_cast<unsigned char>(symbol) < 0x20U) {
      static constexpr const char* const HEX = "0123456789abcdef";

      result += "\\u00";
      result.push_back(HEX[(static_cast<unsigned char>(symbol) >> 4U) & 0xFU]);
      result.push_back(HEX[static_cast<unsigned char>(symbol) & 0xFU]);
    } else {
      result.push_back(symbol);
    }
  }

  return result + "\"";
}

std::string key(const char* const name) { return quoted(name) + ": "; }

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

void Dxxwjz1JsonEmitter::begin(const WeightsHeader& header)
{
  write("{\n  " + key(Dxxwjz1Format::FORMAT_KEY) + quoted(Dxxwjz1Format::NAME) +
        ",\n  " + key(Dxxwjz1Format::VERSION_KEY) +
        std::to_string(Dxxwjz1Format::VERSION) + ",\n  " +
        key(Dxxwjz1Format::GENERATOR_KEY) + quoted(generator()) + ",\n  " +
        key(Dxxwjz1Format::HEADER_KEY) + "{" + key(Dxxwjz1Format::MAJOR_KEY) +
        std::to_string(header.major) + ", " + key(Dxxwjz1Format::MINOR_KEY) +
        std::to_string(header.minor) + ", " + key(Dxxwjz1Format::REVISION_KEY) +
        std::to_string(header.revision) + ", " + key(Dxxwjz1Format::SEEN_KEY) +
        std::to_string(header.seen) + "},\n  " +
        key(Dxxwjz1Format::LAYERS_KEY) + "[");

  mfirst_layer = true;
}

void Dxxwjz1JsonEmitter::layer(const int index, const LAYER_TYPE type,
                               const std::vector<Dxxwjz1ArrayView>& arrays)
{
  write(mfirst_layer ? "\n    {" : ",\n    {");
  mfirst_layer = false;

  write(key(Dxxwjz1Format::INDEX_KEY) + std::to_string(index) + ", " +
        key(Dxxwjz1Format::TYPE_KEY) + quoted(WeightsSchema::name(type)));

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

void Dxxwjz1JsonEmitter::end(const std::vector<uint8_t>& trailing)
{
  write(mfirst_layer ? "]" : "\n  ]");

  if (!trailing.empty()) {
    write(",\n  " + key(Dxxwjz1Format::TRAILING_KEY) +
          quoted(Dxxwjz1Base64::encode(trailing)));
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
