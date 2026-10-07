#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/weights-converter/WeightsConverter.h"

#include <exception>
#include <filesystem>
#include <memory>
#include <random>
#include <string>
#include <system_error>
#include <vector>

#include "src/lib/darknet-adaptor/adaptors/ANetworkAdaptor/ANetworkAdaptor.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/OrigDefaultLoader/OrigDefaultLoader.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/original-weights/OrigWeightsValidator.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/weights-formats/NetworkSource.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/weights-model/NetworkFiles.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/weights-model/WeightsDocument.h"
#include "src/lib/darknet-adaptor/adaptors/NetworksLoaders/weights-model/WeightsReport.h"
#include "src/log/log.h"

namespace darknet_adaptor::loaders
{

namespace
{

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

adaptors::ANetworkAdaptorPtr WeightsConverter::create_network(
    const std::string& cfg)
{
  // the single image batch network of the least memory: the arrays of the
  // weights don't depend on the batch
  return OrigDefaultLoader{}.parse_network_cfg_custom(cfg, 1, 1);
}

bool WeightsConverter::convert(const WeightsConversion& conversion)
{
  if (!check(conversion)) {
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

  if (conversion.stopped()) {
    return false;
  }

  WeightsDocument document;
  const bool sourced = !conversion.src.empty();

  // the network to train of no source file is the description alone
  if ((sourced && !read_source(conversion, net, document)) ||
      !describe(conversion, net, document)) {
    return false;
  }

  // the network to train of a cfg alone, unlike the weights files of the
  // networks of no weights, has no weights file to go back to
  if (sourced && conversion.from->keeps_network() &&
      !conversion.to->keeps_network() && document.network_alone()) {
    LOGE("The " << conversion.src
                << " file is the network to train of no layers, the "
                << conversion.to->name()
                << " file of it would keep nothing but a header");
    return false;
  }

  // the file of no structure keeps no names of its arrays, so the arrays of
  // the document have to follow the network ones
  if (!conversion.to->keeps_structure() && net != nullptr) {
    const WeightsReport report =
        OrigWeightsValidator::validate(document, *net->get());

    report.log(conversion.src);

    if (!report.ok()) {
      return false;
    }
  }

  if (conversion.stopped()) {
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

bool WeightsConverter::check(const WeightsConversion& conversion)
{
  if (conversion.to == nullptr || conversion.dst.empty()) {
    LOGE("The conversion requires the destination file and its format");
    return false;
  }

  if (conversion.from == nullptr && conversion.src.empty()) {
    LOGI("Making the network to train " << conversion.dst << " of the "
                                        << conversion.cfg << " network");

    if (!conversion.to->keeps_network()) {
      LOGE("The " << conversion.to->name()
                  << " file keeps no network description, the network to "
                     "train of no source file is the dxxwjz2 one");
      return false;
    }

    if (conversion.cfg.empty()) {
      LOGE(
          "The network to train of no source file requires the network cfg "
          "file");
      return false;
    }

    return true;
  }

  if (conversion.from == nullptr || conversion.src.empty()) {
    LOGE("The conversion requires both of the source file and its format");
    return false;
  }

  LOGI("Converting the " << conversion.from->name() << " file "
                         << conversion.src << " into the "
                         << conversion.to->name() << " file "
                         << conversion.dst);

  if (!conversion.from->keeps_structure() && conversion.cfg.empty()) {
    LOGE("The " << conversion.from->name()
                << " file keeps no structure of its own, the conversion of it "
                   "requires the network cfg file");
    return false;
  }

  std::error_code error;

  if (!std::filesystem::is_regular_file(conversion.src, error)) {
    LOGE("The source file doesn't exist: " << conversion.src);
    return false;
  }

  return true;
}

bool WeightsConverter::read_source(const WeightsConversion& conversion,
                                   const adaptors::ANetworkAdaptorPtr& net,
                                   WeightsDocument& document)
{
  network* view = net == nullptr ? nullptr : net->get().get();

  if (!conversion.from->keeps_structure() && view != nullptr) {
    // the file keeps no structure, its size tells the file of the network
    const WeightsReport report =
        OrigWeightsValidator::validate(conversion.src, *view);

    report.log(conversion.src);

    if (!report.ok()) {
      return false;
    }
  }

  IWeightsReaderPtr reader = conversion.from->create_reader();

  if (reader == nullptr || !reader->read(conversion.src, view, document)) {
    LOGE("Fail to read the " << conversion.from->name() << " file "
                             << conversion.src);
    return false;
  }

  return true;
}

bool WeightsConverter::describe(const WeightsConversion& conversion,
                                const adaptors::ANetworkAdaptorPtr& net,
                                WeightsDocument& document)
{
  if (!conversion.to->keeps_network()) {
    if (!conversion.names.empty()) {
      LOGW("The " << conversion.to->name() << " file keeps no class names, "
                  << conversion.names << " isn't used");
    }

    return true;
  }

  NetworkDescription source;

  if (!conversion.cfg.empty()) {
    if (!NetworkSource::read(conversion.cfg, source)) {
      return false;
    }

    document.network.cfg(source.cfg());
  }

  if (document.network.empty()) {
    LOGE("The " << conversion.to->name()
                << " file keeps the network description, the conversion "
                   "requires the network cfg file");
    return false;
  }

  // the classes of the dxxwjz2 network file are the source ones
  if (document.network.classes().empty()) {
    document.network.classes(source.classes());
  }

  const std::string names = names_of(conversion, net, document);
  std::vector<std::string> classes;

  if (!names.empty()) {
    if (!NetworkFiles::read_classes(names, classes)) {
      LOGE("Fail to read the class names file " << names);
      return false;
    }

    document.network.classes(classes);
  }

  if (document.network.classes().empty()) {
    LOGW("The class names of the network are unknown, the "
         << conversion.dst << " file keeps none");
  }

  return true;
}

std::string WeightsConverter::names_of(const WeightsConversion& conversion,
                                       const adaptors::ANetworkAdaptorPtr& net,
                                       const WeightsDocument& document)
{
  if (!conversion.names.empty()) {
    return conversion.names;
  }

  std::error_code error;

  // the class names of the source are over the names= key ones
  if (!document.network.classes().empty() || net == nullptr ||
      !std::filesystem::is_regular_file(net->names(), error)) {
    return {};
  }

  return net->names();
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

  IWeightsWriterPtr writer = conversion.to->create_writer();

  const bool written =
      writer != nullptr && writer->write(document, temporary.string());

  if (!written || conversion.stopped()) {
    std::filesystem::remove(temporary, error);

    if (!written) {
      LOGE("Fail to write the " << conversion.to->name() << " file "
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
