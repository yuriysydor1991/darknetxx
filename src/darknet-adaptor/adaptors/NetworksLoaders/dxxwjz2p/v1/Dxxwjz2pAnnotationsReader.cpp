#include "src/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz2p/v1/Dxxwjz2pAnnotationsReader.h"

#include <cstdint>
#include <utility>

#include "src/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz2/v2/Dxxwjz2Format.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz2p/v1/Dxxwjz2pFormat.h"

namespace darknet_adaptor::loaders::dxxwjz2p::v1
{

using F = Dxxwjz2pFormat;

namespace
{

std::string at(const std::string& path, const std::size_t index)
{
  return path + "[" + std::to_string(index) + "]";
}

}  // namespace

Dxxwjz2pAnnotationsReader::Dxxwjz2pAnnotationsReader(
    WeightsReport& nreport, std::filesystem::path ndirectory)
    : Dxxwjz2MetadataReader{nreport}, directory{std::move(ndirectory)}
{
}

bool Dxxwjz2pAnnotationsReader::annotations(
    const json& value, const std::vector<std::string>& classes,
    Yolo4Annotations& annotations)
{
  const std::string path = std::string{"\""} + F::ANNOTATIONS_KEY + "\"";

  mclasses.clear();
  annotations.images.clear();

  for (std::size_t index = 0U; index < classes.size(); ++index) {
    mclasses.emplace(classes[index], static_cast<int>(index));
  }

  if (!object(value, path, {F::ANNOTATIONS_KEY},
              {F::ANNOTATOR_KEY, F::ANNOTATIONS_KEY})) {
    return false;
  }

  const json& groups = value[F::ANNOTATIONS_KEY];

  if (!groups.is_array()) {
    return fail(member(path, F::ANNOTATIONS_KEY) + " isn't an array");
  }

  for (std::size_t index = 0U; index < groups.size(); ++index) {
    if (!group(groups[index], at(member(path, F::ANNOTATIONS_KEY), index),
               annotations)) {
      return false;
    }
  }

  return true;
}

bool Dxxwjz2pAnnotationsReader::group(const json& value,
                                      const std::string& path,
                                      Yolo4Annotations& annotations)
{
  if (!object(value, path, {F::DB_KEY, F::ANNOTATIONS_KEY},
              {F::DB_KEY, F::ANNOTATIONS_KEY}) ||
      !object(value[F::DB_KEY], member(path, F::DB_KEY), {F::PATH_KEY},
              {F::PATH_KEY})) {
    return false;
  }

  const json& db = value[F::DB_KEY][F::PATH_KEY];
  const json& images = value[F::ANNOTATIONS_KEY];

  if (!db.is_string()) {
    return fail(member(member(path, F::DB_KEY), F::PATH_KEY) +
                " isn't a string");
  }

  if (!images.is_array()) {
    return fail(member(path, F::ANNOTATIONS_KEY) + " isn't an array");
  }

  // the relative db path is the one of the directory of the file
  const std::filesystem::path base =
      directory / db.get_ref<const std::string&>();

  for (std::size_t index = 0U; index < images.size(); ++index) {
    Yolo4Image found;

    if (!image(images[index], at(member(path, F::ANNOTATIONS_KEY), index), base,
               found)) {
      return false;
    }

    annotations.images.push_back(std::move(found));
  }

  return true;
}

bool Dxxwjz2pAnnotationsReader::image(const json& value,
                                      const std::string& path,
                                      const std::filesystem::path& db,
                                      Yolo4Image& image)
{
  if (!object(value, path, {F::REL_PATH_KEY, F::ANNOTATIONS_KEY},
              {F::REL_PATH_KEY, F::WIDTH_KEY, F::HEIGHT_KEY,
               F::SCALE_FACTOR_KEY, F::VALIDATION_KEY, F::ANNOTATIONS_KEY}) ||
      !pixels(value, path, F::WIDTH_KEY, image.width) ||
      !pixels(value, path, F::HEIGHT_KEY, image.height)) {
    return false;
  }

  const json& relative = value[F::REL_PATH_KEY];
  const json& rects = value[F::ANNOTATIONS_KEY];
  const json validation = value.value(F::VALIDATION_KEY, json(false));

  if (!relative.is_string() || relative.get_ref<const std::string&>().empty()) {
    return fail(member(path, F::REL_PATH_KEY) + " isn't a path");
  }

  if (!validation.is_boolean()) {
    return fail(member(path, F::VALIDATION_KEY) + " isn't a boolean");
  }

  if (!rects.is_array()) {
    return fail(member(path, F::ANNOTATIONS_KEY) + " isn't an array");
  }

  image.path =
      (db / relative.get_ref<const std::string&>()).lexically_normal().string();
  image.validation = validation.get<bool>();

  if (image.path.size() > Yolo4Image::MAX_PATH) {
    return fail(member(path, F::REL_PATH_KEY) +
                " is the path longer than the darknet ones");
  }
  image.boxes.resize(rects.size());

  for (std::size_t index = 0U; index < rects.size(); ++index) {
    if (!box(rects[index], at(member(path, F::ANNOTATIONS_KEY), index), image,
             image.boxes[index])) {
      return false;
    }
  }

  return true;
}

bool Dxxwjz2pAnnotationsReader::box(const json& value, const std::string& path,
                                    const Yolo4Image& image, Yolo4Box& box)
{
  using dxxwjz2::v2::Dxxwjz2Format;

  if (!object(value, path, {Dxxwjz2Format::NAME_KEY, F::RECT_KEY},
              {Dxxwjz2Format::NAME_KEY, F::RECT_KEY, F::SCALE_FACTOR_KEY}) ||
      !object(value[F::RECT_KEY], member(path, F::RECT_KEY),
              {F::X_KEY, F::Y_KEY, F::WIDTH_KEY, F::HEIGHT_KEY},
              {F::X_KEY, F::Y_KEY, F::WIDTH_KEY, F::HEIGHT_KEY})) {
    return false;
  }

  const json& name = value[Dxxwjz2Format::NAME_KEY];
  const json& rect = value[F::RECT_KEY];
  const auto found = name.is_string()
                         ? mclasses.find(name.get_ref<const std::string&>())
                         : mclasses.end();

  if (found == mclasses.end()) {
    return fail(member(path, Dxxwjz2Format::NAME_KEY) +
                " isn't a class of the network");
  }

  if (image.width <= 0 || image.height <= 0) {
    return fail(path + " is the box of the image of no width and height");
  }

  for (const char* const key :
       {F::X_KEY, F::Y_KEY, F::WIDTH_KEY, F::HEIGHT_KEY}) {
    if (!rect[key].is_number()) {
      return fail(member(member(path, F::RECT_KEY), key) + " isn't a number");
    }
  }

  // the center and the size of the rect, the fractions of the image
  const double width = rect[F::WIDTH_KEY].get<double>();
  const double height = rect[F::HEIGHT_KEY].get<double>();

  box.id = found->second;
  box.x = static_cast<float>((rect[F::X_KEY].get<double>() + width / 2) /
                             image.width);
  box.y = static_cast<float>((rect[F::Y_KEY].get<double>() + height / 2) /
                             image.height);
  box.w = static_cast<float>(width / image.width);
  box.h = static_cast<float>(height / image.height);

  return true;
}

bool Dxxwjz2pAnnotationsReader::pixels(const json& value,
                                       const std::string& path,
                                       const char* const name, int& count)
{
  const json pixels = value.value(name, json(0));

  if (!pixels.is_number_integer() || pixels.get<int64_t>() < 0 ||
      pixels.get<int64_t>() > INT32_MAX) {
    return fail(member(path, name) + " isn't a count of the pixels");
  }

  count = pixels.get<int>();

  return true;
}

}  // namespace darknet_adaptor::loaders::dxxwjz2p::v1
