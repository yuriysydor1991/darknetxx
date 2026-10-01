#include "src/darknet-adaptor/adaptors/NetworksLoaders/weights-converter/WeightsConverter.h"

#include <exception>
#include <filesystem>
#include <memory>
#include <random>
#include <string>
#include <system_error>
#include <vector>

#include "src/darknet-adaptor/adaptors/ANetworkAdaptor/ANetworkAdaptor.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/OrigDefaultLoader/OrigDefaultLoader.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz1/v1/Dxxwjz1Reader.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/dxxwjz1/v1/Dxxwjz1Writer.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/original-weights/OrigWeightsReader.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/original-weights/OrigWeightsValidator.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/original-weights/OrigWeightsWriter.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/weights-model/WeightsDocument.h"
#include "src/darknet-adaptor/adaptors/NetworksLoaders/weights-model/WeightsReport.h"
#include "src/log/log.h"

namespace darknet_adaptor::loaders
{

namespace
{

const std::vector<std::pair<WeightsFormat, std::string>>& names()
{
  static const std::vector<std::pair<WeightsFormat, std::string>> formats{
      {WeightsFormat::Original, "weights"},
      {WeightsFormat::Dxxwjz1, "dxxwjz1"}};

  return formats;
}

bool stopped(const WeightsConversion& conversion)
{
  if (conversion.stop_requested && conversion.stop_requested()) {
    LOGW("The conversion into " << conversion.dst << " is stopped");
    return true;
  }

  return false;
}

/// @brief The temporary file of the destination directory.
std::filesystem::path temporary_of(const std::filesystem::path& dst)
{
  std::random_device random;

  return std::filesystem::path{dst}.concat(".converting-" +
                                           std::to_string(random()));
}

}  // namespace

WeightsConverter::WeightsConverterPtr WeightsConverter::create()
{
  return std::make_shared<WeightsConverter>();
}

const std::vector<std::string>& WeightsConverter::format_names()
{
  static const std::vector<std::string> formats = []() {
    std::vector<std::string> result;

    for (const auto& [format, name] : names()) {
      result.push_back(name);
    }

    return result;
  }();

  return formats;
}

const std::string& WeightsConverter::format_name(const WeightsFormat format)
{
  for (const auto& [known, name] : names()) {
    if (known == format) {
      return name;
    }
  }

  static const std::string unknown{"unknown"};

  return unknown;
}

bool WeightsConverter::parse_format(const std::string& name,
                                    WeightsFormat& format)
{
  for (const auto& [known, known_name] : names()) {
    if (known_name == name) {
      format = known;
      return true;
    }
  }

  return false;
}

IWeightsReaderPtr WeightsConverter::create_reader(const WeightsFormat format)
{
  if (format == WeightsFormat::Dxxwjz1) {
    return dxxwjz1::v1::Dxxwjz1Reader::create();
  }

  return OrigWeightsReader::create();
}

IWeightsWriterPtr WeightsConverter::create_writer(const WeightsFormat format)
{
  if (format == WeightsFormat::Dxxwjz1) {
    return dxxwjz1::v1::Dxxwjz1Writer::create();
  }

  return OrigWeightsWriter::create();
}

adaptors::ANetworkAdaptorPtr WeightsConverter::create_network(
    const std::string& cfg)
{
  // the single image batch network of the least memory: the arrays of the
  // weights don't depend on the batch
  return OrigDefaultLoader{}.parse_network_cfg_custom(cfg, 1, 1);
}

bool WeightsConverter::convert(const WeightsConversion& conversion)
{
  LOGI("Converting the " << format_name(conversion.from) << " file "
                         << conversion.src << " into the "
                         << format_name(conversion.to) << " file "
                         << conversion.dst);

  if (conversion.src.empty() || conversion.dst.empty()) {
    LOGE("The conversion requires both of the source and the destination");
    return false;
  }

  if (conversion.from == WeightsFormat::Original && conversion.cfg.empty()) {
    LOGE(
        "The original weights file keeps no structure of its own, the "
        "conversion of it requires the network cfg file");
    return false;
  }

  std::error_code error;

  if (!std::filesystem::is_regular_file(conversion.src, error)) {
    LOGE("The source file doesn't exist: " << conversion.src);
    return false;
  }

  adaptors::ANetworkAdaptorPtr net;

  if (!conversion.cfg.empty()) {
    try {
      net = create_network(conversion.cfg);
    }
    catch (const std::exception& failure) {
      LOGE("Fail to parse the network cfg file " << conversion.cfg << ": "
                                                 << failure.what());
      return false;
    }
  }

  if (stopped(conversion)) {
    return false;
  }

  WeightsDocument document;

  if (!read_source(conversion, net, document)) {
    return false;
  }

  // the original weights file keeps no names of its arrays, so the arrays of
  // the document have to follow the network ones
  if (conversion.to == WeightsFormat::Original && net != nullptr) {
    const WeightsReport report =
        OrigWeightsValidator::validate(document, *net->get());

    report.log(conversion.src);

    if (!report.ok()) {
      return false;
    }
  }

  if (stopped(conversion)) {
    return false;
  }

  if (!write_destination(conversion, document)) {
    return false;
  }

  LOGI("The conversion into " << conversion.dst
                              << " is complete: " << document.layers.size()
                              << " layers, " << document.floats() << " floats");

  return true;
}

bool WeightsConverter::read_source(const WeightsConversion& conversion,
                                   const adaptors::ANetworkAdaptorPtr& net,
                                   WeightsDocument& document)
{
  network* view = net == nullptr ? nullptr : net->get().get();

  if (conversion.from == WeightsFormat::Original && view != nullptr) {
    // the file keeps no structure, its size tells the file of the network
    const WeightsReport report =
        OrigWeightsValidator::validate(conversion.src, *view);

    report.log(conversion.src);

    if (!report.ok()) {
      return false;
    }
  }

  IWeightsReaderPtr reader = create_reader(conversion.from);

  if (reader == nullptr || !reader->read(conversion.src, view, document)) {
    LOGE("Fail to read the " << format_name(conversion.from) << " file "
                             << conversion.src);
    return false;
  }

  return true;
}

bool WeightsConverter::write_destination(const WeightsConversion& conversion,
                                         const WeightsDocument& document)
{
  const std::filesystem::path dst{conversion.dst};
  std::error_code error;

  if (dst.has_parent_path() &&
      !std::filesystem::exists(dst.parent_path(), error)) {
    std::filesystem::create_directories(dst.parent_path(), error);

    if (error) {
      LOGE("Fail to create the destination directory "
           << dst.parent_path().string() << ": " << error.message());
      return false;
    }
  }

  const std::filesystem::path temporary = temporary_of(dst);

  IWeightsWriterPtr writer = create_writer(conversion.to);

  const bool written =
      writer != nullptr && writer->write(document, temporary.string());

  if (!written || stopped(conversion)) {
    std::filesystem::remove(temporary, error);

    if (!written) {
      LOGE("Fail to write the " << format_name(conversion.to) << " file "
                                << conversion.dst);
    }

    return false;
  }

  std::filesystem::rename(temporary, dst, error);

  if (error) {
    LOGE("Fail to replace the destination " << conversion.dst << ": "
                                            << error.message());
    std::filesystem::remove(temporary, error);
    return false;
  }

  return true;
}

}  // namespace darknet_adaptor::loaders
