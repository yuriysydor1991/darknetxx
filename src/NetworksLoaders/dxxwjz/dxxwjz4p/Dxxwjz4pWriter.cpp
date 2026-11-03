#include "src/NetworksLoaders/dxxwjz/dxxwjz4p/Dxxwjz4pWriter.h"

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <map>
#include <memory>
#include <ostream>
#include <set>
#include <string>
#include <system_error>

#include "src/NetworksLoaders/dxxwjz/dxxwjz4p/Dxxwjz4pFormat.h"
#include "src/NetworksLoaders/dxxwjz/dxxwjz4p/Dxxwjz4pJsonEmitter.h"
#include "src/darknet-adaptor/adaptors/orig-includes.h"

namespace darknet_adaptor::loaders::dxxwjz4p::v1
{

namespace fs = std::filesystem;

namespace
{

/// @brief The level of the staged document, which the archive compresses.
constexpr int STAGING_LEVEL = 0;

std::string lowered(std::string text)
{
  std::transform(text.begin(), text.end(), text.begin(), [](const char c) {
    return static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
  });

  return text;
}

}  // namespace

Dxxwjz4pWriter::Dxxwjz4pWriter(const int level)
    : Dxxwjz3pWriter{STAGING_LEVEL}, mlevel{level}
{
}

Dxxwjz4pWriter::Dxxwjz4pWriterPtr Dxxwjz4pWriter::create()
{
  return std::make_shared<Dxxwjz4pWriter>();
}

bool Dxxwjz4pWriter::save(network& net, const std::string& filename,
                          const int cutoff, const bool save_ema,
                          const WeightsDocument& rest)
{
  plan(rest.annotations, filename);

  const std::string staging = filename + ".json-staging";

  return pack(Dxxwjz3pWriter::save(net, staging, cutoff, save_ema, rest),
              staging, filename);
}

bool Dxxwjz4pWriter::write(const WeightsDocument& document,
                           const std::string& filename)
{
  plan(document.annotations, filename);

  const std::string staging = filename + ".json-staging";

  return pack(Dxxwjz3pWriter::write(document, staging), staging, filename);
}

std::unique_ptr<dxxwjz1::v1::Dxxwjz1JsonEmitter> Dxxwjz4pWriter::create_emitter(
    std::ostream& out, const WeightsDocument& rest, const std::string&) const
{
  return std::make_unique<Dxxwjz4pJsonEmitter>(
      out, rest.network, rest.training, mmembers, mroot, rest.description);
}

bool Dxxwjz4pWriter::check(const WeightsDocument& rest,
                           const std::string& filename) const
{
  if (!Dxxwjz3pWriter::check(rest, filename)) {
    return false;
  }

  for (const Yolo4Image& image : rest.annotations.images) {
    std::error_code error;

    if (!fs::is_regular_file(image.path, error)) {
      CLOGE(
          "The dxxwjz4p project keeps the images of it's annotations, the "
          "image %s is no file",
          image.path.c_str());
      return false;
    }
  }

  return true;
}

void Dxxwjz4pWriter::plan(const Yolo4Annotations& annotations,
                          const std::string& filename)
{
  std::error_code error;
  std::map<std::string, std::string> members;
  std::set<std::string> taken;

  mroot = fs::absolute(filename, error).parent_path();
  mmembers = annotations;
  mimages.clear();

  for (Yolo4Image& image : mmembers.images) {
    auto found = members.find(image.path);

    if (found == members.end()) {
      const fs::path source{image.path};
      const std::string stem = source.stem().string();
      const std::string extension = source.extension().string();
      std::string name = source.filename().string();

      // the same names of the case blind file systems are the repeated ones
      for (int suffix = 1; !taken.insert(lowered(name)).second; ++suffix) {
        name = stem + "_" + std::to_string(suffix) + extension;
      }

      const std::string member =
          std::string{Dxxwjz4pFormat::DATA_FOLDER} + "/" + name;

      found = members.emplace(image.path, member).first;
      mimages.emplace_back(member, source);
    }

    image.path = (mroot / Dxxwjz4pFormat::DATA_FOLDER /
                  fs::path{found->second}.filename())
                     .string();
  }
}

bool Dxxwjz4pWriter::pack(const bool staged, const std::string& staging,
                          const std::string& filename) const
{
  std::error_code error;

  const bool packed =
      staged && Dxxwjz4pArchive::pack(staging, mimages, filename, mlevel);

  fs::remove(staging, error);

  if (staged && !packed) {
    fs::remove(filename, error);
    CLOGE("Couldn't pack the dxxwjz4p project: %s", filename.c_str());
  }

  return packed;
}

}  // namespace darknet_adaptor::loaders::dxxwjz4p::v1
