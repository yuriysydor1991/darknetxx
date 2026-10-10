#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/yolo4-folder/Yolo4FolderReader.h"

#include <exception>
#include <fstream>
#include <locale>
#include <opencv2/imgcodecs.hpp>
#include <system_error>

#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/OrigDefaultLoader/OrigDefaultLoader.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/weights-formats/NetworkSource.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/weights-formats/WeightsFileFormats.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/weights-model/NetworkFiles.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/yolo4-folder/Yolo4FolderLayout.h"
#include "src/log/log.h"
#include "stb_image.h"

namespace darknet_adaptor::loaders::yolo4
{

namespace fs = std::filesystem;

bool Yolo4FolderReader::load(network&, const std::string& filename, int)
{
  LOGE("The YOLO4 folder " << filename
                           << " is the one of the conversion alone");
  return false;
}

bool Yolo4FolderReader::read(const std::string& filename, const network* net,
                             WeightsDocument& document)
{
  std::error_code error;
  const fs::path root = fs::absolute(filename, error);
  const std::string cfg = Yolo4FolderLayout::cfg(root);
  const std::string weights = Yolo4FolderLayout::latest_weights(root, cfg);
  std::string text;

  document = WeightsDocument{};

  if (cfg.empty() || !NetworkFiles::read_text(cfg, text)) {
    LOGE("No cfg/*.cfg network file of the YOLO4 folder " << filename);
    return false;
  }

  if ((!weights.empty() && !read_weights(weights, cfg, net, document)) ||
      !read_classes(root, cfg, document)) {
    return false;
  }

  document.network.cfg(text);

  // the darknet valid= list is the train= one for none
  const std::string train =
      NetworkSource::option_of(cfg, "train", Yolo4FolderLayout::TRAIN);
  const std::string valid = NetworkSource::option_of(cfg, "valid", train);

  return read_list(root, train, false, document.annotations) &&
         (valid == train || read_list(root, valid, true, document.annotations));
}

bool Yolo4FolderReader::read_weights(const std::string& weights,
                                     const std::string& cfg, const network* net,
                                     WeightsDocument& document)
{
  const IWeightsFileFormatPtr format = WeightsFileFormats::of_file(weights);
  adaptors::ANetworkAdaptorPtr own;

  LOGI("Reading the latest weights " << weights << " of the YOLO4 folder");

  try {
    // the weights of no structure of their own are the ones of the cfg
    if (net == nullptr && !format->keeps_structure()) {
      own = OrigDefaultLoader{}.parse_network_cfg_custom(cfg, 1, 1);
      net = own->get().get();
    }
  }
  catch (const std::exception& e) {
    LOGE("Fail to parse the network cfg file " << cfg << ": " << e.what());
    return false;
  }

  return format->create_reader()->read(weights, net, document);
}

bool Yolo4FolderReader::read_classes(const fs::path& root,
                                     const std::string& cfg,
                                     WeightsDocument& document)
{
  std::vector<std::string> classes = document.network.classes();
  std::string names = Yolo4FolderLayout::names(root);
  const fs::path keyed =
      root / NetworkSource::option_of(cfg, "names", "data/names.list");
  std::error_code error;

  // the training reads the names= one for the weights of no class names
  if (names.empty() && classes.empty() && fs::is_regular_file(keyed, error)) {
    names = keyed.string();
  }

  if (!names.empty() && !NetworkFiles::read_classes(names, classes)) {
    LOGE("Couldn't read the class names file " << names);
    return false;
  }

  document.network.classes(classes);

  return true;
}

bool Yolo4FolderReader::read_list(const fs::path& root, const std::string& list,
                                  const bool validation,
                                  Yolo4Annotations& annotations)
{
  std::ifstream file{root / list};
  std::string line;

  if (!file.is_open()) {
    LOGE("Couldn't open the list of the images " << (root / list).string());
    return false;
  }

  while (std::getline(file, line)) {
    // the darknet fgetl() of the CR LF lines as well
    line.erase(line.find_last_not_of(" \t\r") + 1U);

    if (line.size() > Yolo4Image::MAX_PATH) {
      LOGE("The image path of " << list << " longer than the darknet ones");
      return false;
    }

    if (!line.empty()) {
      annotations.images.push_back(read_image(root, line, validation));
    }
  }

  return true;
}

Yolo4Image Yolo4FolderReader::read_image(const fs::path& root,
                                         const std::string& path,
                                         const bool validation)
{
  Yolo4Image image;
  int channels = 0;

  // the darknet label path of the path of the list, the one of the folder
  image.boxes = read_boxes(root / Yolo4Image::label_of(path));
  image.path = (root / path).lexically_normal().string();
  image.validation = validation;

  // the header alone of the stb_image formats, the whole image of the rest
  if (stbi_info(image.path.c_str(), &image.width, &image.height, &channels) ==
      0) {
    const cv::Mat pixels = cv::imread(image.path, cv::IMREAD_UNCHANGED);

    image.width = pixels.cols;
    image.height = pixels.rows;
  }

  if (image.width <= 0 || image.height <= 0) {
    LOGW("Couldn't read the size of the image " << image.path);
  }

  return image;
}

std::vector<Yolo4Box> Yolo4FolderReader::read_boxes(const fs::path& label)
{
  std::ifstream file{label};
  std::vector<Yolo4Box> boxes;
  Yolo4Box box;

  if (!file.is_open()) {
    LOGW("No label file " << label.string() << ", the image of no object");
  }

  file.imbue(std::locale::classic());

  while (file >> box.id >> box.x >> box.y >> box.w >> box.h) {
    boxes.push_back(box);
  }

  return boxes;
}

}  // namespace darknet_adaptor::loaders::yolo4
