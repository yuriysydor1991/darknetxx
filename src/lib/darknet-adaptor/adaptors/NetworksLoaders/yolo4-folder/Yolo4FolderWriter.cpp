#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/yolo4-folder/Yolo4FolderWriter.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <fstream>
#include <sstream>
#include <system_error>

#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz1/v1/Dxxwjz1FloatCodec.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz3/v3/Dxxwjz3Writer.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/yolo4-folder/Yolo4FolderLayout.h"
#include "src/log/log.h"

namespace darknet_adaptor::loaders::yolo4
{

namespace fs = std::filesystem;

using L = Yolo4FolderLayout;

bool Yolo4FolderWriter::save(network&, const std::string& filename, int, bool)
{
  LOGE("The YOLO4 folder " << filename
                           << " is the one of the conversion alone");
  return false;
}

bool Yolo4FolderWriter::write(const WeightsDocument& document,
                              const std::string& filename)
{
  const fs::path root{filename};
  // the temporary folder of the converter is of the destination name
  const std::string name = root.filename().string();
  const std::string stem = name.substr(0, name.find('.'));
  const std::string base = stem.empty() ? "yolov4-obj" : stem;
  const std::string valid =
      std::any_of(document.annotations.images.begin(),
                  document.annotations.images.end(),
                  [](const Yolo4Image& image) { return image.validation; })
          ? L::VALID
          : L::TRAIN;
  std::map<std::string, std::string> taken;
  std::string lists[2];
  std::string names;
  std::error_code error;

  for (const std::string& folder : {L::CFG_DIR, L::IMAGES_DIR, L::BACKUP}) {
    if (!fs::create_directories(root / folder, error) && error) {
      LOGE("Fail to create the folder " << (root / folder).string() << ": "
                                        << error.message());
      return false;
    }
  }

  for (const Yolo4Image& image : document.annotations.images) {
    if (!copy(image, root, taken, lists[image.validation ? 1 : 0])) {
      return false;
    }
  }

  for (const std::string& classname : document.network.classes()) {
    names += classname + "\n";
  }

  return write_text(root / L::TRAIN, lists[0]) &&
         (valid == L::TRAIN || write_text(root / L::VALID, lists[1])) &&
         write_text(root / L::NAMES, names) &&
         write_text(
             root / L::DATA,
             "classes=" + std::to_string(document.network.classes().size()) +
                 "\ntrain=" + L::TRAIN + "\nvalid=" + valid +
                 "\nnames=" + L::NAMES + "\nbackup=" + L::BACKUP + "/\n") &&
         write_text(
             root / L::CFG_DIR / (base + ".cfg"),
             net_keys(document.network.cfg(), {{"train", L::TRAIN},
                                               {"valid", valid},
                                               {"names", L::NAMES},
                                               {"backup", L::BACKUP}})) &&
         (document.network_alone() ||
          dxxwjz3::v3::Dxxwjz3Writer{}.write(
              document,
              (root / L::BACKUP / (base + "_last.dxxwjz3")).string()));
}

bool Yolo4FolderWriter::copy(const Yolo4Image& image, const fs::path& root,
                             std::map<std::string, std::string>& taken,
                             std::string& list)
{
  const fs::path source{image.path};
  const std::string stem = source.stem().string();
  std::string name = stem;
  std::string label;
  std::error_code error;
  bool copied = false;

  // the label files of the stems, of no case of the case blind file systems
  for (int index = 1;; name = stem + "_" + std::to_string(index++)) {
    std::string lower = name;

    std::transform(lower.begin(), lower.end(), lower.begin(),
                   [](const unsigned char c) {
                     return static_cast<char>(std::tolower(c));
                   });

    const auto [found, added] = taken.emplace(lower, image.path);

    // the image of both of the lists is copied once
    copied = !added && found->second == image.path;

    if (added || copied) {
      break;
    }
  }

  const std::string path =
      L::IMAGES_DIR + "/" + name + source.extension().string();
  const std::string labelpath = Yolo4Image::label_of(path);

  list += path + "\n";

  if (copied) {
    return true;
  }

  if (labelpath.size() < 4U ||
      labelpath.compare(labelpath.size() - 4U, 4U, ".txt") != 0) {
    LOGE("The image " << image.path << " of no darknet label file name");
    return false;
  }

  for (const Yolo4Box& box : image.boxes) {
    using dxxwjz1::v1::Dxxwjz1FloatCodec;

    if (!std::isfinite(box.x + box.y + box.w + box.h)) {
      LOGE("The box of no finite values of the image " << image.path);
      return false;
    }

    label += std::to_string(box.id) + " " + Dxxwjz1FloatCodec::encode(box.x) +
             " " + Dxxwjz1FloatCodec::encode(box.y) + " " +
             Dxxwjz1FloatCodec::encode(box.w) + " " +
             Dxxwjz1FloatCodec::encode(box.h) + "\n";
  }

  if (!fs::copy_file(source, root / path, error)) {
    LOGE("Fail to copy the image " << image.path << ": " << error.message());
    return false;
  }

  return write_text(root / labelpath, label);
}

bool Yolo4FolderWriter::write_text(const fs::path& file,
                                   const std::string& text)
{
  std::ofstream out{file, std::ios::binary};

  out << text;
  out.close();

  if (!out) {
    LOGE("Fail to write the file " << file.string());
  }

  return static_cast<bool>(out);
}

std::string Yolo4FolderWriter::net_keys(
    const std::string& cfg, const std::map<std::string, std::string>& keys)
{
  std::istringstream lines{cfg};
  std::map<std::string, std::string> rest = keys;
  std::string text;
  std::string line;
  int sections = 0;

  const auto end_section = [&text, &rest]() {
    for (const auto& [key, value] : rest) {
      text += key + "=" + value + "\n";
    }

    rest.clear();
  };

  while (std::getline(lines, line)) {
    // the darknet read_cfg() line of no white space
    std::string option = line;

    option.erase(std::remove_if(option.begin(), option.end(),
                                [](const unsigned char c) {
                                  return std::isspace(c) != 0;
                                }),
                 option.end());

    const auto key = keys.find(option.substr(0, option.find('=')));

    if (!option.empty() && option[0] == '[' && ++sections == 2) {
      end_section();
    }

    if (sections == 1 && key != keys.end() &&
        option.find('=') != std::string::npos) {
      // the first line of the key alone, the darknet reads that one
      text += rest.erase(key->first) > 0U
                  ? key->first + "=" + key->second + "\n"
                  : std::string{};
      continue;
    }

    text += line + "\n";
  }

  end_section();

  return text;
}

}  // namespace darknet_adaptor::loaders::yolo4
